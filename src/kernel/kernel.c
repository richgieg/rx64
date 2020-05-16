#include "loader_info.h"
#include "console.h"
#include "graphics.h"
#include "kernel.h"

VOID
KernelEntry (
    IN LOADER_INFO *LoaderInfo
    )
{
    GfxInitializeGraphics(
        LoaderInfo->Graphics.FrameBufferBase,
        LoaderInfo->Graphics.FrameBufferSize,
        LoaderInfo->Graphics.HorizontalResolution,
        LoaderInfo->Graphics.VerticalResolution
    );

    // CnInitializeConsole();

    // for (int i = 0; i < 13440; i++) {
    //     CnPrint(L"A");
    // }

    // UINT32 Color = 0;

    // for (;;) {
    //     CnPrint(L"A");
    //     CnSetForegroundColor(Color++);
    // }

    GfxFillScreen(255);
    GfxFillBlockOnScreen(800, 600, 160, 160, 0xff00);

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
