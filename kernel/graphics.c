#include <graphics.h>

static GFX_FRAME_BUFFER mScreenBuffer;

VOID
GfxInitializeGraphics (
    IN UINT64 FrameBufferBase,
    IN UINT32 HorizontalResolution,
    IN UINT32 VerticalResolution
    )
{
    mScreenBuffer.Base = FrameBufferBase;
    mScreenBuffer.Size = (UINT64)HorizontalResolution *
        VerticalResolution * 4; // 4 bytes per pixel
    mScreenBuffer.HorizontalResolution = HorizontalResolution;
    mScreenBuffer.VerticalResolution = VerticalResolution;
}

VOID
GfxFillBuffer (
    IN CONST GFX_FRAME_BUFFER   *Buffer,
    IN UINT32                   Color
    )
{
    UINT32 *Pixel;
    UINT32 *MaxPixel;

    Pixel = (UINT32 *)Buffer->Base;
    MaxPixel =  (UINT32 *)(Buffer->Base + Buffer->Size);
    while (Pixel < MaxPixel) {
        *Pixel = Color;
        Pixel++;
    }
}

VOID
GfxFillScreen (
    IN UINT32 Color
    )
{
    GfxFillBuffer(&mScreenBuffer, Color);
}

UINT32
GfxGetHorizontalResolution ()
{
    return mScreenBuffer.HorizontalResolution;
}

UINT32
GfxGetVerticalResolution ()
{
    return mScreenBuffer.VerticalResolution;
}
