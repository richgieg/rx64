#include <loader_info.h>
#include <intrin.h>
#include <types.h>
#include <graphics_.h>
#include <console.h>
#include <console_.h>
#include <memory_.h>

void kmain (
    LOADER_INFO *LoaderInfo
    )
{
    GfxInitializeGraphics(LoaderInfo->GraphicsInfo);
    CnInitializeConsole();

    CnPrint(L"Rx64 v0.0.1\n");
    CnPrint(L"(c) 2020 Richard Gieg\n");

    MmInitializeMemory(LoaderInfo->MemoryInfo);

    for (;;) {
        __halt();
    }
}
