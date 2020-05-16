#include "graphics.h"
#include "memory.h"
#include "runtime.h"

static GFX_FRAME_BUFFER mScreenBuffer;

VOID
GfxInitializeGraphics (
    IN UINTN  FrameBufferBase,
    IN UINTN  FrameBufferSize,
    IN UINT32 HorizontalResolution,
    IN UINT32 VerticalResolution
    )
{
    mScreenBuffer.Base = FrameBufferBase;
    mScreenBuffer.Size = FrameBufferSize;
    mScreenBuffer.HorizontalResolution = HorizontalResolution;
    mScreenBuffer.VerticalResolution = VerticalResolution;
}

GFX_FRAME_BUFFER *
GfxCreateBuffer ()
{
    GFX_FRAME_BUFFER *FrameBuffer;
    FrameBuffer = MmAllocatePool(sizeof(GFX_FRAME_BUFFER));
    FrameBuffer->HorizontalResolution = GfxGetHorizontalResolution();
    FrameBuffer->VerticalResolution = GfxGetVerticalResolution();
    FrameBuffer->Size = mScreenBuffer.HorizontalResolution * mScreenBuffer.VerticalResolution * 4;
    FrameBuffer->Base = (UINTN)MmAllocatePages(mScreenBuffer.Size / MM_PAGE_SIZE);
}

VOID
GfxBltLinesInBuffer (
    IN CONST GFX_FRAME_BUFFER   *FrameBuffer,
    IN UINT32                   DestinationY,
    IN UINT32                   SourceY,
    IN UINTN                    NoLines
    )
{
    VOID *Source;
    VOID *Destination;
    UINTN Length;

    Source = ((UINT32 *)FrameBuffer->Base) + (SourceY * FrameBuffer->HorizontalResolution);
    Destination = ((UINT32 *)FrameBuffer->Base) + (DestinationY * FrameBuffer->HorizontalResolution);
    Length = NoLines * FrameBuffer->HorizontalResolution * sizeof(UINT32);
    RtCopyMemory(Destination, Source, Length);
}

VOID
GfxBltLinesOnScreen (
    IN UINT32                   DestinationY,
    IN UINT32                   SourceY,
    IN UINTN                    NoLines
    )
{
    GfxBltLinesInBuffer(&mScreenBuffer, DestinationY, SourceY, NoLines);
}

VOID
GfxDrawBitmap (
    IN UINT32       X,
    IN UINT32       Y,
    IN UINT32       Width,
    IN UINT32       Height,
    IN UINT32       mForegroundColor,
    IN UINT32       mBackgroundColor,
    IN CONST UINT8  *SrcBuffer
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
        Pixel = ((UINT32 *)mScreenBuffer.Base) + ((i + Y) * mScreenBuffer.HorizontalResolution) + X;
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
GfxFillBlockInBuffer (
    IN CONST GFX_FRAME_BUFFER   *FrameBuffer,
    IN UINT32                   X,
    IN UINT32                   Y,
    IN UINT32                   Width,
    IN UINT32                   Height,
    IN UINT32                   Color
    )
{
    UINTN i;
    UINTN j;
    UINT32 *Pixel;

    for (i = 0; i < Height; i++) {
        Pixel = ((UINT32 *)FrameBuffer->Base) + ((i + Y) * FrameBuffer->HorizontalResolution) + X;
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
    IN CONST GFX_FRAME_BUFFER   *FrameBuffer,
    IN UINT32                   Color
    )
{
    UINT32 *Pixel;
    UINT32 *MaxPixel;
    
    Pixel = (UINT32 *)FrameBuffer->Base;
    MaxPixel =  (UINT32 *)FrameBuffer->Base + FrameBuffer->Size;
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

VOID
GfxCopyBuffer (
    IN CONST GFX_FRAME_BUFFER *DestinationBuffer,
    IN CONST GFX_FRAME_BUFFER *SourceBuffer
    )
{
    RtCopyMemory((VOID *)DestinationBuffer->Base,
        (VOID *)SourceBuffer->Base, DestinationBuffer->Size);
}

VOID
GfxCopyBufferToScreen (
    IN CONST GFX_FRAME_BUFFER *SourceBuffer
    )
{
    GfxCopyBuffer(&mScreenBuffer, SourceBuffer);
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

VOID
GfxPlayDrawMemoryAsBitmapDemo ()
{
    UINT8 *Memory = 0;

    while (1) {
        GfxDrawBitmap(0, 0, mScreenBuffer.HorizontalResolution, mScreenBuffer.VerticalResolution,
            0x0000ff00, 0, Memory);
        Memory += mScreenBuffer.HorizontalResolution * mScreenBuffer.VerticalResolution / 8;
    }
}
