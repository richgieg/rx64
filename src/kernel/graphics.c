#include "graphics.h"

static UINT64 mFrameBufferBase;
static UINT64 mFrameBufferSize;
static UINT32 mHorizontalResolution;
static UINT32 mVerticalResolution;

VOID
InitializeGraphics (
    UINT64 FrameBufferBase,
    UINT64 FrameBufferSize,
    UINT32 HorizontalResolution,
    UINT32 VerticalResolution
    )
{
    mFrameBufferBase = FrameBufferBase;
    mFrameBufferSize = FrameBufferSize;
    mHorizontalResolution = HorizontalResolution;
    mVerticalResolution = VerticalResolution;
}

VOID
DrawBitmap (
    UINT32          X,
    UINT32          Y,
    UINT32          Width,
    UINT32          Height,
    UINT32          mForegroundColor,
    UINT32          mBackgroundColor,
    const UINT8     *SrcBuffer
    )
{
    UINTN   i;
    UINTN   j;
    UINT32  *Pixel;
    UINT8   Bits;
    UINTN   BitCounter;

    Bits = *SrcBuffer;
    BitCounter = 0;

    for (i = 0; i < Height; i++) {
        Pixel = ((UINT32 *)mFrameBufferBase) + ((i + Y) * mHorizontalResolution) + X;
        for (j = 0; j < Width; j++) {
            *Pixel = (Bits & 0x80) ? mForegroundColor : mBackgroundColor;
            Bits = Bits << 1;
            Pixel++;
            BitCounter++;
            if (BitCounter == 8) {
                SrcBuffer++;
                Bits = *SrcBuffer;
                BitCounter = 0;
            }
        }
    }
}

VOID
FillBlock (
    UINT32 X,
    UINT32 Y,
    UINT32 Width,
    UINT32 Height,
    UINT32 Color
    )
{
    UINTN i;
    UINTN j;
    UINT32 *Pixel;

    for (i = 0; i < Height; i++) {
        Pixel = ((UINT32 *)mFrameBufferBase) + ((i + Y) * mHorizontalResolution) + X;
        for (j = 0; j < Width; j++) {
            *Pixel = Color;
            Pixel++;
        }
    }
}

VOID
FillScreen (
    UINT32 Color
    )
{
    UINT32 *Pixel;
    UINT32 *MaxPixel;

    Pixel = (UINT32 *)mFrameBufferBase;
    MaxPixel =  (UINT32 *)mFrameBufferBase + mFrameBufferSize;
    while (Pixel < MaxPixel) {
        *Pixel = Color;
        Pixel++;
    }
}

UINT32
GetHorizontalResolution ()
{
    return mHorizontalResolution;
}

UINT32
GetVerticalResolution ()
{
    return mHorizontalResolution;
}

VOID
PlayDrawMemoryAsBitmapDemo ()
{
    UINT8 *Memory = 0;

    while (1) {
        DrawBitmap(0, 0, mHorizontalResolution, mVerticalResolution,
            0x0000ff00, 0, Memory);
        Memory += mHorizontalResolution * mVerticalResolution / 8;
    }
}
