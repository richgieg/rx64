#include <memory.h>
#include <memory_.h>
#include <console.h>
#include <debug.h>
#include <loader_info.h>
#include <runtime.h>

// Array of page allocation lists. Each list corresponds to a
// physical memory range (see mUsableRanges). List entries are
// sorted by physical address (ascending). Entries where
// PhysicalAddress is NULL are placed at the end.
static MM_PAGE_ALLOCATION           **mPhysicalAllocationLists;

// List of page allocations sorted by virtual address (ascending).
// Entries where VirtualAddress is NULL are at the end.
static MM_PAGE_ALLOCATION           *mVirtualAllocationList;

// Init pool variables.
static UINT64                       mInitPoolAddress;
static UINT64                       mInitPoolNumPages;
static UINT64                       mInitPoolNextAddress;
static BOOLEAN                      mInitPoolInitialized;

// Array of usable physical memory ranges supplied by loader.
static LOADER_USABLE_MEMORY_RANGE   *mUsableRanges;
static UINT64                       mNumUsableRanges;

VOID
MmInitializeMemory (
    IN LOADER_MEMORY_INFO *MemoryInfo
    )
{
    UINT64                  Size;
    MM_PAGE_ALLOCATION      *NewEntry;

    // Initialize the init pool using reserved range from loader.
    mInitPoolAddress = MemoryInfo->ReservedRangePhysicalAddress;
    mInitPoolNumPages = MemoryInfo->NumPagesInReservedRange;
    mInitPoolNextAddress = mInitPoolAddress;
    mInitPoolInitialized = TRUE;

    // Copy usable physical memory ranges from loader.
    mNumUsableRanges = MemoryInfo->NumUsableRanges;
    Size = mNumUsableRanges * sizeof(LOADER_USABLE_MEMORY_RANGE);
    mUsableRanges = MmAllocateInitPool(Size);
    RtCopyMemory(mUsableRanges, MemoryInfo->UsableRanges, Size);

    // Initialize physical allocation lists.
    mPhysicalAllocationLists = MmAllocateInitPool(sizeof(MM_PAGE_ALLOCATION *) * MemoryInfo->NumUsableRanges);
    for (UINT64 i = 0; i < MemoryInfo->NumUsableRanges; i++) {
        mPhysicalAllocationLists[i] = NULL;
    }

    // Initialize the first page allocation entry.
    // The entry is for the init pool.
    NewEntry = MmAllocateInitPool(sizeof(MM_PAGE_ALLOCATION));
    NewEntry->PhysicalAddress = mInitPoolAddress;
    // Virtual address will be mapped later.
    NewEntry->VirtualAddress = (UINT64)NULL;
    NewEntry->NumPages = mInitPoolNumPages;
    // No other allocation entries yet.
    NewEntry->NextPhysical = NULL;
    NewEntry->NextVirtual = NULL;

    MmInsertIntoPhysicalAllocationLists(NewEntry);
}

VOID
MmInsertIntoPhysicalAllocationLists (
    IN MM_PAGE_ALLOCATION *Entry
    )
{
    UINT64              FirstAddressInEntry;
    UINT64              LastAddressInEntry;
    UINT64              FirstAddressInRange;
    UINT64              LastAddressInRange;
    BOOLEAN             RangeFound;
    MM_PAGE_ALLOCATION  *List;
    UINT64              i;

    if (Entry->PhysicalAddress == (UINT64)NULL) {
        DbgHalt(L"MmInsertIntoPhysicalAllocationLists: Physical address cannot be null.");
    }

    FirstAddressInEntry = Entry->PhysicalAddress;
    LastAddressInEntry = FirstAddressInEntry + (Entry->NumPages * MM_PAGE_SIZE) - 1;

    // TODO: Handle case where Entry already in list.

    // Determine physical memory range to which entry belongs.
    RangeFound = FALSE;
    for (i = 0; i < mNumUsableRanges; i++) {
        FirstAddressInRange = mUsableRanges[i].PhysicalAddress;
        LastAddressInRange = FirstAddressInRange + (mUsableRanges[i].NumPages * MM_PAGE_SIZE) - 1;
        if (FirstAddressInEntry >= FirstAddressInRange && LastAddressInEntry <= LastAddressInRange) {
            RangeFound = TRUE;
            break;
        }
    }
    if (!RangeFound) {
        DbgHalt(L"MmInsertIntoPhysicalAllocationLists: No physical range found that can accommodate entry.");
    }
    // If list has no entries then this entry will be the first.
    if (mPhysicalAllocationLists[i] == NULL) {
        mPhysicalAllocationLists[i] = Entry;
    // Otherwise, find position within list to add entry.
    } else {
        DbgHalt(L"MmInsertIntoPhysicalAllocationLists: Not implemented.");
        List = mPhysicalAllocationLists[i];
        CnPrintHex((UINT64)List);
    }
}

//VOID
//MmInitializeMemory_old (
//    IN LOADER_MEMORY_INFO *MemoryInfo
//    )
//{
//    MM_PAGE_ALLOCATION      *NewEntry;
//    MM_PAGE_ALLOCATION      *CurrentEntry;
//    MM_PAGE_ALLOCATION      *PreviousEntry;
//    UINT64                  Size;
//    UINT64                  i;
//
//    // Initialize the init pool using reserved range from loader.
//    mInitPoolAddress = MemoryInfo->ReservedRangePhysicalAddress;
//    mInitPoolNumPages = MemoryInfo->NumPagesInReservedRange;
//    mInitPoolNextAddress = mInitPoolAddress;
//    mInitPoolInitialized = TRUE;
//
//    // Initialize the first page allocation entry.
//    // The entry is for the init pool.
//    NewEntry = MmAllocateInitPool(sizeof(MM_PAGE_ALLOCATION));
//    NewEntry->PhysicalAddress = mInitPoolAddress;
//    // Virtual address will be mapped later.
//    NewEntry->VirtualAddress = (UINT64)NULL;
//    NewEntry->NumPages = mInitPoolNumPages;
//    // No other allocation entries yet.
//    NewEntry->NextPhysical = NULL;
//    NewEntry->NextVirtual = NULL;
//
//    // Point the physical and virtual list heads to the first entry.
//    mPhysicalAllocationList = NewEntry;
//    mVirtualAllocationList = NewEntry;
//
//    // Create an allocation entry for each mapping from loader
//    // and insert it into the physical and virtual allocation
//    // lists at the necessary position to maintain sort order.
//    for (i = 0; i < MemoryInfo->NumMappings; i++) {
//
//        NewEntry = MmAllocateInitPool(sizeof(MM_PAGE_ALLOCATION));
//        // This is here because static analyzer complains about
//        // dereferencing a null pointer immediately after this
//        // "if" statement if it's not there. There's probably a
//        // better way to handle this but I don't know it yet...
//        if (NewEntry == NULL) {
//            DbgHalt(L"MmInitializeMemory: Received null pointer from MmAllocateInitPool.");
//        }
//        NewEntry->PhysicalAddress = MemoryInfo->Mappings[i].PhysicalAddress;
//        NewEntry->VirtualAddress = MemoryInfo->Mappings[i].VirtualAddress;
//        NewEntry->NumPages = MemoryInfo->Mappings[i].NumPages;
//        NewEntry->NextPhysical = NULL;
//        NewEntry->NextVirtual = NULL;
//
//        // Insert allocation entry into physical allocation list
//        // while maintaining ascending sort order of physical addresses.
//        PreviousEntry = NULL;
//        CurrentEntry = mPhysicalAllocationList;
//        while ((CurrentEntry != NULL) &&
//            (CurrentEntry->PhysicalAddress != (UINT64)NULL) &&
//            (NewEntry->PhysicalAddress >= CurrentEntry->PhysicalAddress)
//        ) {
//            PreviousEntry = CurrentEntry;
//            CurrentEntry = CurrentEntry->NextPhysical;
//        }
//        NewEntry->NextPhysical = CurrentEntry;
//        if (PreviousEntry != NULL) {
//            PreviousEntry->NextPhysical = NewEntry;
//        } else {
//            mPhysicalAllocationList = NewEntry;
//        }
//
//        // Insert allocation entry into virtual allocation list
//        // while maintaining ascending sort order of virtual addresses.
//        PreviousEntry = NULL;
//        CurrentEntry = mVirtualAllocationList;
//        while ((CurrentEntry != NULL) &&
//            (CurrentEntry->VirtualAddress != (UINT64)NULL) &&
//            (NewEntry->VirtualAddress>= CurrentEntry->VirtualAddress)
//        ) {
//            PreviousEntry = CurrentEntry;
//            CurrentEntry = CurrentEntry->NextVirtual;
//        }
//        NewEntry->NextVirtual = CurrentEntry;
//        if (PreviousEntry != NULL) {
//            PreviousEntry->NextVirtual = NewEntry;
//        } else {
//            mVirtualAllocationList = NewEntry;
//        }
//
//    }
//
//    // Copy usable physical memory ranges from loader.
//    mNumUsableRanges = MemoryInfo->NumUsableRanges;
//    Size = mNumUsableRanges * sizeof(LOADER_USABLE_MEMORY_RANGE);
//    mUsableRanges = MmAllocateInitPool(Size);
//    RtCopyMemory(mUsableRanges, MemoryInfo->UsableRanges, Size);
//
//    MmAllocatePhysicalPages(1000);
//}

VOID *
MmAllocateInitPool (
    IN UINT64 Size
    )
{
    VOID *Address;

    if (!mInitPoolInitialized) {
        DbgHalt(L"MmAllocateInitPool: Not initialized yet.");
    }
    if ((mInitPoolNextAddress + Size) >= (mInitPoolAddress + (mInitPoolNumPages * MM_PAGE_SIZE))) {
        DbgHalt(L"MmAllocateInitPool: Not enough space.");
    }
    Address = (VOID *)mInitPoolNextAddress;
    mInitPoolNextAddress += Size;
    return Address;
}

VOID *
MmAllocatePhysicalPages (
    IN UINT64 NumPages
    )
{
    //MM_PAGE_ALLOCATION  *CurrentEntry;
    //UINT64              i;
    //UINT64              MinAddress;
    //UINT64              MaxAddress;

    //CnPrint(L"\n");
    //PrintPhysicalAllocations();
    //CnPrint(L"\n");
    //PrintUsableRanges();
    //CnPrint(L"\n");

    //CurrentEntry = mPhysicalAllocationList;

    //for (i = 0; i < mNumUsableRanges; i++) {
    //    // If requested pages can't fit in the usable range then skip it.
    //    if (NumPages > mUsableRanges[i].NumPages) {
    //        continue;
    //    }
    //    // Walk the physical allocation list and try to find an adequate slot.
    //    MinAddress = mUsableRanges[i].PhysicalAddress;
    //    MaxAddress = MinAddress + ((mUsableRanges[i].NumPages - NumPages) * MM_PAGE_SIZE);

    //    CnPrintHex(MinAddress);
    //    CnPrint(L" ");
    //    CnPrintHex(MaxAddress);
    //    CnPrint(L"\n");
    //}

    return NULL;
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

VOID
PrintPhysicalAllocations ()
{
    MM_PAGE_ALLOCATION *CurrentEntry;
    UINT64 i;

    CnPrint(L"Physical Allocations\n");
    CnPrint(L"--------------------\n");
    for (i = 0; i < mNumUsableRanges; i++) {
        CurrentEntry = mPhysicalAllocationLists[i];
        while (CurrentEntry != NULL) {
            CnPrint(L"Address: ");
            CnPrintHexWithPad(CurrentEntry->PhysicalAddress, 16);
            CnPrint(L"  Pages: ");
            CnPrintHex(CurrentEntry->NumPages);
            CnPrint(L"\n");
            CurrentEntry = CurrentEntry->NextPhysical;
        }
    }
}

VOID
PrintUsableRanges ()
{
    UINT64 i;

    CnPrint(L"Usable Ranges\n");
    CnPrint(L"-------------\n");
    for (i = 0; i < mNumUsableRanges; i++) {
        CnPrint(L"Address: ");
        CnPrintHexWithPad(mUsableRanges[i].PhysicalAddress, 16);
        CnPrint(L"  Pages: ");
        CnPrintHex(mUsableRanges[i].NumPages);
        CnPrint(L"\n");
    }
}

#endif
