#include "../shared/loader_info.h"
#include "graphics.h"

void
KernelEntry (
    LOADER_INFO *LoaderInfo
    )
{
    InitializeGraphics(
        LoaderInfo->Graphics.FrameBufferBase,
        LoaderInfo->Graphics.FrameBufferSize
    );

    FillScreen(0x00ff0000);
    FillChar(0, 0, 0x000000ff);
    FillChar(239, 55, 0x000000ff);

    for (;;) {
        asm("hlt");
    }
}
