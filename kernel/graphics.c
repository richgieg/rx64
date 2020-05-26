#include <graphics.h>
#include <graphics_.h>
#include <runtime.h>

static GFX_FRAME_BUFFER mScreenBuffer;

VOID
GfxInitializeGraphics (
    LOADER_GRAPHICS_INFO *GraphicsInfo
    )
{
    mScreenBuffer.Base = GraphicsInfo->FrameBufferBase;
    mScreenBuffer.Size = (UINT64)GraphicsInfo->HorizontalResolution *
        GraphicsInfo->VerticalResolution * 4; // 4 bytes per pixel
    mScreenBuffer.HorizontalResolution = GraphicsInfo->HorizontalResolution;
    mScreenBuffer.VerticalResolution = GraphicsInfo->VerticalResolution;
}

VOID
GfxBltLinesInBuffer (
    IN CONST GFX_FRAME_BUFFER   *Buffer,
    IN UINT32                   DestinationY,
    IN UINT32                   SourceY,
    IN UINT64                   NoLines
    )
{
    VOID    *Source;
    VOID    *Destination;
    UINT64  Length;

    Source = ((UINT32 *)Buffer->Base) + ((UINT64)SourceY * Buffer->HorizontalResolution);
    Destination = ((UINT32 *)Buffer->Base) + ((UINT64)DestinationY * Buffer->HorizontalResolution);
    Length = NoLines * Buffer->HorizontalResolution * sizeof(UINT32);
    RtCopyMemory(Destination, Source, Length);
}

VOID
GfxBltLinesOnScreen (
    IN UINT32                   DestinationY,
    IN UINT32                   SourceY,
    IN UINT64                   NoLines
    )
{
    GfxBltLinesInBuffer(&mScreenBuffer, DestinationY, SourceY, NoLines);
}

VOID
GfxDrawBitmapInBuffer (
    IN CONST GFX_FRAME_BUFFER   *Buffer,
    IN UINT32                   X,
    IN UINT32                   Y,
    IN UINT32                   Width,
    IN UINT32                   Height,
    IN UINT32                   ForegroundColor,
    IN UINT32                   BackgroundColor,
    IN CONST UINT8              *Bitmap
    )
{
    UINT64  i;
    UINT64  j;
    UINT32  *Pixel;
    UINT8   Bits;
    UINT64  BitCounter;

    Bits = *Bitmap;
    BitCounter = 0;

    for (i = 0; i < Height; i++) {
        Pixel = ((UINT32 *)Buffer->Base) + ((i + Y) * Buffer->HorizontalResolution) + X;
        for (j = 0; j < Width; j++) {
            *Pixel = (Bits & 0x80) ? ForegroundColor : BackgroundColor;
            Bits = Bits << 1;
            Pixel++;
            BitCounter++;
            if (BitCounter == 8) {
                Bitmap++;
                Bits = *Bitmap;
                BitCounter = 0;
            }
        }
    }
}

VOID
GfxDrawBitmapOnScreen (
    IN UINT32                   X,
    IN UINT32                   Y,
    IN UINT32                   Width,
    IN UINT32                   Height,
    IN UINT32                   ForegroundColor,
    IN UINT32                   BackgroundColor,
    IN CONST UINT8              *Bitmap
    )
{
    GfxDrawBitmapInBuffer(&mScreenBuffer, X, Y, Width, Height,
        ForegroundColor, BackgroundColor, Bitmap);
}

VOID
GfxFillBlockInBuffer (
    IN CONST GFX_FRAME_BUFFER   *Buffer,
    IN UINT32                   X,
    IN UINT32                   Y,
    IN UINT32                   Width,
    IN UINT32                   Height,
    IN UINT32                   Color
    )
{
    UINT64 i;
    UINT64 j;
    UINT32 *Pixel;

    for (i = 0; i < Height; i++) {
        Pixel = ((UINT32 *)Buffer->Base) + ((i + Y) * Buffer->HorizontalResolution) + X;
        for (j = 0; j < Width; j++) {
            *Pixel = Color;
            Pixel++;
        }
    }
}

VOID
GfxFillBlockOnScreen (
    IN UINT32 X,
    IN UINT32 Y,
    IN UINT32 Width,
    IN UINT32 Height,
    IN UINT32 Color
    )
{
    GfxFillBlockInBuffer(&mScreenBuffer, X, Y, Width, Height, Color);
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
