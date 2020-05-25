#include <intrin.h>
#include <kmain.h>

void kmain (
    LOADER_INFO *LoaderInfo
    )
{
    unsigned __int32 *Pixel;
    unsigned __int32 MaxPixel;
    unsigned __int32 i;

    Pixel = (unsigned __int32 *)LoaderInfo->GraphicsInfo->FrameBufferBase;
    MaxPixel = LoaderInfo->GraphicsInfo->HorizontalResolution *
        LoaderInfo->GraphicsInfo->VerticalResolution;
    for (i = 0; i < MaxPixel; i++) {
        Pixel[i] = 0xff00;
    }
    
    for (;;) {
        __halt();
    }
}
