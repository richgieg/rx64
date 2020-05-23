// Very naive memory manager... will be improved!

#include "kernel.h"
#include "memory.h"

static UINT64   mContiguousPhysicalMemoryBase;
static UINTN    mNoPages;
static UINT64   mBasePoolAddress;
static UINT64   mNextPoolAddress;
static UINT64   mBasePageAddress;
static UINT64   mNextPageAddress;

VOID
MmInitialize (
    UINT64  ContiguousPhysicalMemoryBase,
    UINTN   NoPages
    )
{
    mContiguousPhysicalMemoryBase = ContiguousPhysicalMemoryBase;
    mNoPages = NoPages;
    // Pool is 1 MB.
    mBasePoolAddress = mContiguousPhysicalMemoryBase;
    mNextPoolAddress = mBasePoolAddress;
    // Start page allocations after the pool.
    mBasePageAddress = mBasePoolAddress + 0x100000;
    mNextPageAddress = mBasePageAddress;
}

VOID *
MmAllocatePages (
    UINTN NoPages
    )
{
    VOID *Address;
    Address = (VOID *)mNextPageAddress;
    mNextPageAddress += NoPages * MM_PAGE_SIZE;
    return Address;
}

VOID
MmZeroPages (
    VOID    *Pages,
    UINTN   NoPages
    )
{
    UINT64  *ZeroWriter;
    UINT64  *Max;

    ZeroWriter = (UINT64 *)Pages;
    Max = (UINT64 *)(Pages + (NoPages * MM_PAGE_SIZE));

    while (ZeroWriter < Max) {
        *ZeroWriter++ = 0;
    }
}

VOID *
MmAllocatePool (
    UINTN Size
    )
{
    // Ensure that new pool allocation doesn't run into page allocations.
    if (mNextPoolAddress + Size >= mBasePageAddress) {
        KeBugCheck();
    }
    VOID *Address;
    Address = (VOID *)mNextPoolAddress;
    mNextPoolAddress += Size;
    return Address;
}

UINT64
MmGetPml4TableAddress ()
{
    UINT64 Pml4TableAddress;
    UINT64 CR3;

    asm(
        "mov %%cr3, %%rax;"
        "mov %%rax, %0;"
        :"=r"(CR3)
        :
        :"%rax"
    );
    // Ignore bits 0 through 11 (PWT and PCD bits, etc.).
    Pml4TableAddress = CR3 & 0xfffffffffffff000;
    return Pml4TableAddress;
}

VOID
MmMapVirtualToPhysicalPages (
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress,
    IN UINTN    NoPages
    )
{
    UINTN i;

    for (i = 0; i < NoPages; i++) {
        MmMapVirtualToPhysicalPage(VirtualAddress, PhysicalAddress);
        VirtualAddress += MM_PAGE_SIZE;
        PhysicalAddress += MM_PAGE_SIZE;
    }
}

VOID
MmMapVirtualToPhysicalPage (
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress
    )
{
    UINT64      *Pml4;
    UINT64      *Pdpt;
    UINT64      *Pd;
    UINT64      *Pt;
    UINTN       Pml4Index;
    UINTN       PdptIndex;
    UINTN       PdIndex;
    UINTN       PtIndex;

    Pml4 = (UINT64 *)MmGetPml4TableAddress();
    Pml4Index = (VirtualAddress >> 39) & 0x1ff;
    PdptIndex = (VirtualAddress >> 30) & 0x1ff;
    PdIndex = (VirtualAddress >> 21) & 0x1ff;
    PtIndex = (VirtualAddress >> 12) & 0x1ff;

    // If PML4 entry not present then allocate a page for its
    // corresponding page directory pointer table.
    if (!(Pml4[Pml4Index] & 1)) {
        Pdpt = MmAllocatePages(1);
        MmZeroPages(Pdpt, 1);
        Pml4[Pml4Index] = (UINT64)Pdpt | 0x23;
    // Otherwise, get page directory pointer table address from it.
    } else {
        Pdpt = (UINT64 *)(Pml4[Pml4Index] & 0x00fffffffffffff000);
    }

    // If page directory pointer table entry not present then
    // allocate a page for its corresponding page directory.
    if (!(Pdpt[PdptIndex] & 1)) {
        Pd = MmAllocatePages(1);
        MmZeroPages(Pd, 1);
        Pdpt[PdptIndex] = (UINT64)Pd | 0x23;
    // Otherwise, get page directory address from it.
    } else {
        Pd = (UINT64 *)(Pdpt[PdptIndex] & 0x00fffffffffffff000);
    }

    // If page directory entry not present then allocate a page
    // for its corresponding page table.
    if (!(Pd[PdIndex] & 1)) {
        Pt = MmAllocatePages(1);
        MmZeroPages(Pt, 1);
        Pd[PdIndex] = (UINT64)Pt | 0x23;
    // Otherwise, get page table address from it.
    } else {
        Pt = (UINT64 *)(Pd[PdIndex] & 0x00fffffffffffff000);
    }

    // Write entry to page table.
    Pt[PtIndex] = PhysicalAddress | 0x23;
}
