#include <graphics.h>
#include <intrin.h>
#include <kmain.h>
#include <types.h>

void kmain (
    LOADER_INFO *LoaderInfo
    )
{
    GfxInitializeGraphics(
        LoaderInfo->GraphicsInfo->FrameBufferBase,
        LoaderInfo->GraphicsInfo->HorizontalResolution,
        LoaderInfo->GraphicsInfo->VerticalResolution);
    
    GfxFillScreen(0xff);

    for (;;) {
        __halt();
    }
}
