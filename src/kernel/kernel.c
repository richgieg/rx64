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

    FillScreen(0x00ff0000);
    FillBlock(0, 0, 1920, 540, 0x000000ff);

    for (;;) {
        asm("hlt");
    }
}
