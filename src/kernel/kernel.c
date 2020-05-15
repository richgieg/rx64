#include "loader_info.h"
#include "console.h"
#include "graphics.h"
#include "kernel.h"

VOID
KernelEntry (
    LOADER_INFO *LoaderInfo
    )
{
    GfxInitializeGraphics(
        LoaderInfo->Graphics.FrameBufferBase,
        LoaderInfo->Graphics.FrameBufferSize,
        LoaderInfo->Graphics.HorizontalResolution,
        LoaderInfo->Graphics.VerticalResolution
    );

    CnInitializeConsole();

    for (int i = 0; i < 13439; i++) {
        CnPrint(L"A");
    }

    for (;;) {
        asm("hlt");
    }
}

VOID
KeBugCheck ()
{
    // Red Screen of Destruction...
    GfxFillScreen(0x00ff0000);

    for (;;) {
        asm("hlt");
    }
}
