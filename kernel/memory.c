#include <memory.h>
#include <memory_.h>
#include <console.h>

VOID
MmInitializeMemory (
    IN LOADER_MEMORY_INFO *MemoryInfo
    )
{
    PrintLoaderMemoryInfo(MemoryInfo);
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
