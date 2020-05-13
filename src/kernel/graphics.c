#include "graphics.h"

static unsigned long mFrameBufferBase;
static unsigned long mFrameBufferSize;

void
InitializeGraphics (
    unsigned long FrameBufferBase,
    unsigned long FrameBufferSize
    )
{
    mFrameBufferBase = FrameBufferBase;
    mFrameBufferSize = FrameBufferSize;
}

void
FillScreen (
    unsigned int Color
    )
{
    unsigned int *Pixel;
    unsigned int *MaxPixel;

    Pixel = (unsigned int *)mFrameBufferBase;
    MaxPixel =  (unsigned int *)mFrameBufferBase + mFrameBufferSize;
    while (Pixel < MaxPixel) {
        *Pixel = Color;
        Pixel++;
    }
}

void
FillChar (
    unsigned int    Column,
    unsigned int    Row,
    unsigned int    Color
    )
{
    unsigned int *Pixel;
    unsigned int PixelsPerScanLine;
    unsigned int Width;
    unsigned int Height;
    unsigned int i;
    unsigned int j;

    PixelsPerScanLine = 1920;
    Width = 8;
    Height = 19;

    for (i = 0; i < Height; i++) {
        Pixel = ((unsigned int *)mFrameBufferBase) + (i * PixelsPerScanLine) +
            (Row * Height * PixelsPerScanLine) + (Column * Width);
        for (j = 0; j < Width; j++) {
            *Pixel = Color;
            Pixel++;
        }
    }
}
