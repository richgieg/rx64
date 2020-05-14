#include "loader_info.h"
#include "console.h"
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

    InitializeConsole();

    PutChar(1, 0, 'C');

    for (;;) {
        asm("hlt");
    }
}
