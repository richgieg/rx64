#include <efi.h>
#include <efilib.h>
#include "intrin.h"
#include "memory.h"
#include "memory_.h"

#define MAX_MAPPINGS            1000
#define MAX_AVAILABLE_RANGES    1000

static LOADER_MEMORY_MAPPING    *mMappings;
static UINTN                    mNumMappings;

EFI_STATUS
GetMemoryInfo (
    OUT LOADER_MEMORY_INFO  **MemoryInfo,
    OUT UINTN               *MemoryMapKey
    )
{
    LOADER_AVAILABLE_MEMORY_RANGE   *AvailableRanges;
    UINTN                           NumAvailableRanges;
    EFI_MEMORY_DESCRIPTOR           *MemoryMap;
    EFI_MEMORY_DESCRIPTOR           *MemoryMapEntry;
    UINTN                           NumEntries;
    UINTN                           DescriptorSize;
    UINT32                          DescriptorVersion;
    UINT64                          PhysicalEndOfPreviousRange;

    *MemoryInfo = AllocatePool(sizeof(LOADER_MEMORY_INFO));
    AvailableRanges = AllocatePool(
        sizeof(LOADER_AVAILABLE_MEMORY_RANGE) * MAX_AVAILABLE_RANGES);
    NumAvailableRanges = 0;
    MemoryMap = LibMemoryMap(&NumEntries, MemoryMapKey, &DescriptorSize, &DescriptorVersion);
    MemoryMapEntry = MemoryMap;

    for (UINTN i = 0; i < NumEntries; i++) {
        switch (MemoryMapEntry->Type) {
            case EfiBootServicesCode:
            case EfiBootServicesData:
            case EfiLoaderCode:
            case EfiLoaderData:
            case EfiConventionalMemory: {
                // Fail if exhausted maximum number of ranges.
                if (NumAvailableRanges >= MAX_AVAILABLE_RANGES) {
                    Print(L"GetMemoryInfo: Exhausted maximum number of available ranges\n");
                    return EFI_ABORTED;
                }
                // Fail if memory map entries aren't sorted.
                // TODO: Sort the entries?
                if (NumAvailableRanges > 0 && MemoryMapEntry->PhysicalStart < PhysicalEndOfPreviousRange) {
                    Print(L"GetMemoryInfo: Memory map entries not sorted\n");
                    return EFI_ABORTED;
                }
                // Consolidate range with previous one if they are contiguous.
                if (NumAvailableRanges > 0 && MemoryMapEntry->PhysicalStart == PhysicalEndOfPreviousRange) {
                    AvailableRanges[NumAvailableRanges - 1].NumPages += MemoryMapEntry->NumberOfPages;
                // Otherwise, start a new entry.
                } else {
                    AvailableRanges[NumAvailableRanges].PhysicalAddress = MemoryMapEntry->PhysicalStart;
                    AvailableRanges[NumAvailableRanges].NumPages = MemoryMapEntry->NumberOfPages;
                    NumAvailableRanges++;
                }
                PhysicalEndOfPreviousRange = MemoryMapEntry->PhysicalStart +
                    (MemoryMapEntry->NumberOfPages * EFI_PAGE_SIZE);
                break;
            }
        }
        MemoryMapEntry = NextMemoryDescriptor(MemoryMapEntry, DescriptorSize);
    }

    (*MemoryInfo)->Mappings = mMappings;
    (*MemoryInfo)->NumMappings = mNumMappings;
    (*MemoryInfo)->AvailableRanges = AvailableRanges;
    (*MemoryInfo)->NumAvailableRanges = NumAvailableRanges;

    return EFI_SUCCESS;
}

EFI_STATUS
MapMemory (
    IN LOADER_MEMORY_MAPPING_TYPE   Type,
    IN UINT64                       PhysicalAddress,
    IN UINT64                       VirtualAddress,
    IN UINTN                        NumPages
    )
{
    EFI_STATUS  Status;
    UINTN       i;

    // On first call, allocate memory for MEMORY_MAPPING records.
    if (mMappings == NULL) {
        mMappings = AllocatePool(sizeof(LOADER_MEMORY_MAPPING) * MAX_MAPPINGS);
        ZeroMem(mMappings,sizeof(LOADER_MEMORY_MAPPING) * MAX_MAPPINGS);
    }
    // Fail if exhausted maximum number of mappings.
    if (mNumMappings >= MAX_MAPPINGS) {
        Print(L"MapMemory: Exhausted maximum number of mappings\n");
        return EFI_ABORTED;
    }
    // Map the pages.
    for (i = 0; i < NumPages * EFI_PAGE_SIZE; i += EFI_PAGE_SIZE) {
        Status = MapPage(VirtualAddress + i, PhysicalAddress + i);
        if (EFI_ERROR(Status)) {
            return Status;
        }
    }
    // Add mapping record.
    mMappings[mNumMappings].Type = Type;
    mMappings[mNumMappings].PhysicalAddress = PhysicalAddress;
    mMappings[mNumMappings].VirtualAddress = VirtualAddress;
    mMappings[mNumMappings].NumPages = NumPages;
    mNumMappings++;

    return EFI_SUCCESS;
}

EFI_STATUS
MapPage (
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
            Print(L"MapPage: Failed to allocate page for PDPT\n");
            return Status;
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
            Print(L"MapPage: Failed to allocate page for PD\n");
            return Status;
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
            Print(L"MapPage: Failed to allocate page for PT\n");
            return Status;
        }
        ZeroMem(Pt, EFI_PAGE_SIZE);
        Pd[PdIndex] = (UINT64)Pt | 0x23;
        // Otherwise, get page table address from it.
    } else {
        Pt = (UINT64 *)(Pd[PdIndex] & 0x00fffffffffffff000);
    }

    // Write entry to page table.
    Pt[PtIndex] = PhysicalAddress | 0x23;

    return EFI_SUCCESS;
}
