#include <intrin.h>

void kmain (
    unsigned __int64 FrameBufferBase,
    unsigned __int32 HorizontalResolution,
    unsigned __int32 VerticalResolution
    )
{
    unsigned __int32 *Pixel;
    unsigned __int32 i;

    Pixel = (unsigned __int32 *)FrameBufferBase;

    for (i = 0; i < HorizontalResolution * VerticalResolution;  i++) {
        Pixel[i] = 0xff0000;
    }

    for (;;) {
        __halt();
    }

}
