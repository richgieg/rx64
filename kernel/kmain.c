#include <kmain.h>
#include <intrin.h>
#include <types.h>
#include <graphics_.h>
#include <console.h>
#include <console_.h>
#include <runtime.h>

void kmain (
    LOADER_INFO *LoaderInfo
    )
{
    GfxInitializeGraphics(
        LoaderInfo->GraphicsInfo->FrameBufferBase,
        LoaderInfo->GraphicsInfo->HorizontalResolution,
        LoaderInfo->GraphicsInfo->VerticalResolution);
    
    CnInitializeConsole();

    CnPrint(L"Rx64 v0.0.1\n");
    CnPrint(L"(c) 2020 Richard Gieg\n");

    CnPrint(L"\n\n");
    for (UINT64 i = 0; i < LoaderInfo->MemoryInfo->NumMappings; i++) {
        CnPrint(L"Type: ");
        CnPrintHex(LoaderInfo->MemoryInfo->Mappings[i].Type);
        CnPrint(L"  Physical: ");
        CnPrintHexWithPad(LoaderInfo->MemoryInfo->Mappings[i].PhysicalAddress, 16);
        CnPrint(L"  Virtual: ");
        CnPrintHexWithPad(LoaderInfo->MemoryInfo->Mappings[i].VirtualAddress, 16);
        CnPrint(L"  Pages: ");
        CnPrintHex(LoaderInfo->MemoryInfo->Mappings[i].NumPages);
        CnPrint(L"\n");
    }

    CnPrint(L"\n");
    for (UINT64 i = 0; i < LoaderInfo->MemoryInfo->NumAvailableRanges; i++) {
        CnPrint(L"Start: ");
        CnPrintHexWithPad(LoaderInfo->MemoryInfo->AvailableRanges[i].PhysicalAddress, 16);
        CnPrint(L"  End: ");
        CnPrintHexWithPad(LoaderInfo->MemoryInfo->AvailableRanges[i].PhysicalAddress + 
            (LoaderInfo->MemoryInfo->AvailableRanges[i].NumPages * 4096) - 1, 16);
        CnPrint(L"  Pages: ");
        CnPrintHex(LoaderInfo->MemoryInfo->AvailableRanges[i].NumPages);
        CnPrint(L"\n");
    }

    for (;;) {
        __halt();
    }
}
