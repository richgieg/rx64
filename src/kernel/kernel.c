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

    for (int i = 0; i < 13439; i++) {
        Print(L"A");
    }

    for (;;) {
        asm("hlt");
    }
}
