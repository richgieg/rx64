#include "graphics.h"

static UINT64 mFrameBufferBase;
static UINT64 mFrameBufferSize;

VOID
InitializeGraphics (
    UINT64 FrameBufferBase,
    UINT64 FrameBufferSize
    )
{
    mFrameBufferBase = FrameBufferBase;
    mFrameBufferSize = FrameBufferSize;
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

VOID
FillChar (
    UINT16 Column,
    UINT16 Row,
    UINT32 Color
    )
{
    UINT32 *Pixel;
    UINTN PixelsPerScanLine;
    UINTN Width;
    UINTN Height;
    UINTN i;
    UINTN j;

    PixelsPerScanLine = 1920;
    Width = 8;
    Height = 19;

    for (i = 0; i < Height; i++) {
        Pixel = ((UINT32 *)mFrameBufferBase) + (i * PixelsPerScanLine) +
            (Row * Height * PixelsPerScanLine) + (Column * Width);
        for (j = 0; j < Width; j++) {
            *Pixel = Color;
            Pixel++;
        }
    }
}
