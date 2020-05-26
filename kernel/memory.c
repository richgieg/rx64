#include <memory_.h>
#include <console.h>

VOID
MmInitializeMemory (
    LOADER_MEMORY_INFO *MemoryInfo
    )
{
    UINT64 UsedPages;
    UINT64 TotalPages;

    UsedPages = 0;
    TotalPages = 0;

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
            UsedPages += MemoryInfo->Mappings[i].NumPages;
        }
    }

    CnPrint(L"\n");
    for (UINT64 i = 0; i < MemoryInfo->NumAvailableRanges; i++) {
        CnPrint(L"Start: ");
        CnPrintHexWithPad(MemoryInfo->AvailableRanges[i].PhysicalAddress, 16);
        CnPrint(L"  End: ");
        CnPrintHexWithPad(MemoryInfo->AvailableRanges[i].PhysicalAddress + 
            (MemoryInfo->AvailableRanges[i].NumPages * 4096) - 1, 16);
        CnPrint(L"  Pages: ");
        CnPrintHex(MemoryInfo->AvailableRanges[i].NumPages);
        CnPrint(L"\n");
        TotalPages = TotalPages + MemoryInfo->AvailableRanges[i].NumPages;
    }

    CnPrint(L"\nTotal Bytes:     ");
    CnPrintHex(TotalPages * 4096);
    CnPrint(L"\nBytes Used:      ");
    CnPrintHex(UsedPages * 4096);
    CnPrint(L"\nBytes Available: ");
    CnPrintHex((TotalPages - UsedPages) * 4096);
}
