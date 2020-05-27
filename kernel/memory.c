#include <memory.h>
#include <memory_.h>
#include <console.h>
#include <debug.h>
#include <loader_info.h>
#include <runtime.h>

static MM_PAGE_ALLOCATION           mFirstPhysicalAllocation;
static MM_PAGE_ALLOCATION           mFirstVirtualAllocation;
static UINT64                       mInitPoolAddress;
static UINT64                       mInitPoolSize;
static UINT64                       mInitPoolNextAddress;
static BOOLEAN                      mInitPoolInitialized;
static LOADER_USABLE_MEMORY_RANGE   *mUsableRanges;
static UINT64                       mNumUsableRanges;

VOID
MmInitializeMemory (
    IN LOADER_MEMORY_INFO *MemoryInfo
    )
{
    UINT64                  Size;
    LOADER_MEMORY_MAPPING   *LoaderMemoryMappings;
    UINT64                  NumLoaderMemoryMappings;

    // Initialize the "init pool" using reserved range from loader.
    mInitPoolAddress = MemoryInfo->ReservedRangePhysicalAddress;
    mInitPoolSize = MemoryInfo->NumPagesInReservedRange * MM_PAGE_SIZE;
    mInitPoolNextAddress = mInitPoolAddress;
    mInitPoolInitialized = TRUE;

    // Copy usable physical memory ranges from loader.
    mNumUsableRanges = MemoryInfo->NumUsableRanges;
    Size = mNumUsableRanges * sizeof(LOADER_USABLE_MEMORY_RANGE);
    mUsableRanges = MmAllocateInitPool(Size);
    RtCopyMemory(mUsableRanges, MemoryInfo->UsableRanges, Size);

    // Copy memory mappings from loader.
    NumLoaderMemoryMappings = MemoryInfo->NumMappings;
    Size = NumLoaderMemoryMappings * sizeof(LOADER_MEMORY_MAPPING);
    LoaderMemoryMappings = MmAllocateInitPool(Size);
    RtCopyMemory(LoaderMemoryMappings, MemoryInfo->Mappings, Size);


    //mFirstPhysicalAllocation.PhysicalAddress = MemoryInfo->ReservedRangePhysicalAddress;
    //mFirstPhysicalAllocation.VirtualAddress = (UINT64)NULL;
    //mFirstPhysicalAllocation.NumPages = MemoryInfo->NumPagesInReservedRange;
    //mFirstPhysicalAllocation.NextPhysical = NULL;
    //mFirstPhysicalAllocation.NextVirtual = NULL;
}

VOID *
MmAllocateInitPool (
    IN UINT64 Size
    )
{
    VOID *Address;

    if (!mInitPoolInitialized) {
        DbgHalt(L"MmAllocateInitPool: Not initialized yet.");
    }
    if ((mInitPoolNextAddress + Size) >= (mInitPoolAddress + mInitPoolSize)) {
        DbgHalt(L"MmALlocateInitPool: Not enough space.");
    }
    Address = (VOID *)mInitPoolNextAddress;
    mInitPoolNextAddress += Size;
    return Address;
}

#ifdef _DEBUG

VOID
PrintLoaderMemoryInfo (
    IN LOADER_MEMORY_INFO *MemoryInfo
    )
{
    UINT64 NumUsedPages;
    UINT64 NumUsablePages;

    NumUsedPages = 0;
    NumUsablePages = 0;

    CnPrint(L"\n");
    for (UINT64 i = 0; i < MemoryInfo->NumMappings; i++) {
        CnPrint(L"Type: ");
        CnPrintHex(MemoryInfo->Mappings[i].Type);
        CnPrint(L"  Physical: ");
        CnPrintHexWithPad(MemoryInfo->Mappings[i].PhysicalAddress, 16);
        CnPrint(L"  Virtual: ");
        CnPrintHexWithPad(MemoryInfo->Mappings[i].VirtualAddress, 16);
        CnPrint(L"  Pages: ");
        CnPrintHex(MemoryInfo->Mappings[i].NumPages);
        CnPrint(L"\n");

        if (MemoryInfo->Mappings[i].Type == LoaderKernelMemoryMapping) {
            NumUsedPages += MemoryInfo->Mappings[i].NumPages;
        }
    }

    CnPrint(L"\n");
    for (UINT64 i = 0; i < MemoryInfo->NumUsableRanges; i++) {
        CnPrint(L"Start: ");
        CnPrintHexWithPad(MemoryInfo->UsableRanges[i].PhysicalAddress, 16);
        CnPrint(L"  End: ");
        CnPrintHexWithPad(MemoryInfo->UsableRanges[i].PhysicalAddress + 
            (MemoryInfo->UsableRanges[i].NumPages * MM_PAGE_SIZE) - 1, 16);
        CnPrint(L"  Pages: ");
        CnPrintHex(MemoryInfo->UsableRanges[i].NumPages);
        CnPrint(L"\n");
        NumUsablePages = NumUsablePages + MemoryInfo->UsableRanges[i].NumPages;
    }

    CnPrint(L"\nStats (bytes):");
    CnPrint(L"\nUsable:     ");
    CnPrintHex(NumUsablePages * MM_PAGE_SIZE);
    CnPrint(L"\nUsed:       ");
    CnPrintHex(NumUsedPages * MM_PAGE_SIZE);
    CnPrint(L"\nReserved:   ");
    CnPrintHex(MemoryInfo->NumPagesInReservedRange * MM_PAGE_SIZE);
    CnPrint(L"\nAvailable:  ");
    CnPrintHex((NumUsablePages - NumUsedPages -
        MemoryInfo->NumPagesInReservedRange) * MM_PAGE_SIZE);
}

#endif
