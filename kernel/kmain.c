#include <intrin.h>
#include <kmain.h>
#include <types.h>

void kmain (
    LOADER_INFO *LoaderInfo
    )
{
    UINT32 *Pixel;
    UINT32 MaxPixel;
    UINT32 i;

    Pixel = (UINT32 *)LoaderInfo->GraphicsInfo->FrameBufferBase;
    MaxPixel = LoaderInfo->GraphicsInfo->HorizontalResolution *
        LoaderInfo->GraphicsInfo->VerticalResolution;
    for (i = 0; i < MaxPixel; i++) {
        Pixel[i] = 0xff00;
    }
    
    for (;;) {
        __halt();
    }
}
