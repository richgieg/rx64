#include "loader_info.h"
#include "console.h"
#include "graphics.h"
#include "kernel.h"
#include "memory.h"

VOID
KernelEntry (
    IN LOADER_INFO *LoaderInfo
    )
{
    MmInitialize();

    GfxInitializeGraphics(
        LoaderInfo->Graphics.FrameBufferBase,
        LoaderInfo->Graphics.FrameBufferSize,
        LoaderInfo->Graphics.HorizontalResolution,
        LoaderInfo->Graphics.VerticalResolution
    );

    CnInitializeConsole();

    UINT32 Color = 0;
    for (;;) {
        CnSetForegroundColor(Color);
        CnPrint(L"A");
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
