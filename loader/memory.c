#include <efi.h>
#include <efilib.h>
#include "memory.h"

VOID
MapVirtualToPhysicalPages (
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress,
    IN UINTN    NumPages
    )
{
    UINTN i;

    for (i = 0; i < NumPages; i++) {
        MapVirtualToPhysicalPage(VirtualAddress, PhysicalAddress);
        VirtualAddress += EFI_PAGE_SIZE;
        PhysicalAddress += EFI_PAGE_SIZE;
    }
}

VOID
MapVirtualToPhysicalPage (
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress
    )
{
    EFI_STATUS  Status;
    UINT64      *Pml4;
    UINT64      *Pdpt;
    UINT64      *Pd;
    UINT64      *Pt;
    UINTN       Pml4Index;
    UINTN       PdptIndex;
    UINTN       PdIndex;
    UINTN       PtIndex;

    // Address of PML4 is in CR3 register.
    Pml4 = (UINT64 *)__readcr3();
    Pml4Index = (VirtualAddress >> 39) & 0x1ff;
    PdptIndex = (VirtualAddress >> 30) & 0x1ff;
    PdIndex = (VirtualAddress >> 21) & 0x1ff;
    PtIndex = (VirtualAddress >> 12) & 0x1ff;

    // If PML4 entry not present then allocate a page for its
    // corresponding page directory pointer table.
    if (!(Pml4[Pml4Index] & 1)) {
        Status = BS->AllocatePages(AllocateAnyPages, EfiLoaderData, 1, (EFI_PHYSICAL_ADDRESS *)&Pdpt);
        if (EFI_ERROR(Status)) {
            Print(L"Failed to allocate page for PDPT\n");
            Exit(EFI_SUCCESS, 0, NULL);
        }
        ZeroMem(Pdpt, EFI_PAGE_SIZE);
        Pml4[Pml4Index] = (UINT64)Pdpt | 0x23;
        // Otherwise, get page directory pointer table address from it.
    } else {
        Pdpt = (UINT64 *)(Pml4[Pml4Index] & 0x00fffffffffffff000);
    }

    // If page directory pointer table entry not present then
    // allocate a page for its corresponding page directory.
    if (!(Pdpt[PdptIndex] & 1)) {
        Status = BS->AllocatePages(AllocateAnyPages, EfiLoaderData, 1, (EFI_PHYSICAL_ADDRESS *)&Pd);
        if (EFI_ERROR(Status)) {
            Print(L"Failed to allocate page for PD\n");
            Exit(EFI_SUCCESS, 0, NULL);
        }
        ZeroMem(Pd, EFI_PAGE_SIZE);
        Pdpt[PdptIndex] = (UINT64)Pd | 0x23;
        // Otherwise, get page directory address from it.
    } else {
        Pd = (UINT64 *)(Pdpt[PdptIndex] & 0x00fffffffffffff000);
    }

    // If page directory entry not present then allocate a page
    // for its corresponding page table.
    if (!(Pd[PdIndex] & 1)) {
        Status = BS->AllocatePages(AllocateAnyPages, EfiLoaderData, 1, (EFI_PHYSICAL_ADDRESS *)&Pt);
        if (EFI_ERROR(Status)) {
            Print(L"Failed to allocate page for PT\n");
            Exit(EFI_SUCCESS, 0, NULL);
        }
        ZeroMem(Pt, EFI_PAGE_SIZE);
        Pd[PdIndex] = (UINT64)Pt | 0x23;
        // Otherwise, get page table address from it.
    } else {
        Pt = (UINT64 *)(Pd[PdIndex] & 0x00fffffffffffff000);
    }

    // Write entry to page table.
    Pt[PtIndex] = PhysicalAddress | 0x23;
}
