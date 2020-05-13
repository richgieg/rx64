#include "loader_info.h"
#include "graphics.h"

VOID
KernelEntry (
    LOADER_INFO *LoaderInfo
    )
{
    InitializeGraphics(
        LoaderInfo->Graphics.FrameBufferBase,
        LoaderInfo->Graphics.FrameBufferSize,
        LoaderInfo->Graphics.HorizontalResolution,
        LoaderInfo->Graphics.VerticalResolution
    );

    PlayDrawMemoryAsBitmapDemo();

    for (;;) {
        asm("hlt");
    }
}
