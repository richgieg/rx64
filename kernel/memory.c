#include <memory_.h>
#include <console.h>

VOID
MmInitializeMemory (
    LOADER_MEMORY_INFO *MemoryInfo
    )
{
    CnPrint(L"\n\n");
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
    }
}
