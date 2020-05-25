#include <kmain.h>
#include <intrin.h>
#include <types.h>
#include <graphics.h>
#include <console.h>

void kmain (
    LOADER_INFO *LoaderInfo
    )
{
    GfxInitializeGraphics(
        LoaderInfo->GraphicsInfo->FrameBufferBase,
        LoaderInfo->GraphicsInfo->HorizontalResolution,
        LoaderInfo->GraphicsInfo->VerticalResolution);
    
    CnInitializeConsole();

    for (;;) {
        __halt();
    }
}
