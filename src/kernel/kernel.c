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

    CnInitializeConsole();

    UINT32 Color = 0;
    for (;;) {
        CnPrint(L"AAAAAAAAAAA\n");
        CnPrint(L"AAAAAAAAAAAAAAAAA\n");
        CnPrint(L"AAAAAAAAAAAAAAAAAAAAAAAA\n");
        CnSetForegroundColor(Color);
        Color++;
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
