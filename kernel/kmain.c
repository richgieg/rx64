#include <kmain.h>
#include <intrin.h>
#include <types.h>
#include <graphics.h>
#include <console.h>
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

    for (;;) {
        __halt();
    }
}
