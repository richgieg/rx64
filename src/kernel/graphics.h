#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "types.h"

typedef struct _GFX_FRAME_BUFFER {
    UINTN   Base;
    UINTN   Size;
    UINT32  HorizontalResolution;
    UINT32  VerticalResolution;
} GFX_FRAME_BUFFER;

VOID
GfxInitializeGraphics (
    IN UINTN  FrameBufferBase,
    IN UINTN  FrameBufferSize,
    IN UINT32 HorizontalResolution,
    IN UINT32 VerticalResolution
    );

VOID
GfxBltLines (
    IN UINT32   DestinationY,
    IN UINT32   SourceY,
    IN UINTN    NoLines
    );

VOID
GfxDrawBitmap (
    IN UINT32       X,
    IN UINT32       Y,
    IN UINT32       Width,
    IN UINT32       Height,
    IN UINT32       mForegroundColor,
    IN UINT32       mBackgroundColor,
    IN CONST UINT8  *SrcBuffer
    );

VOID
GfxFillBlockInBuffer (
    IN CONST GFX_FRAME_BUFFER   *FrameBuffer,
    IN UINT32                   X,
    IN UINT32                   Y,
    IN UINT32                   Width,
    IN UINT32                   Height,
    IN UINT32                   Color
    );

VOID
GfxFillBlockOnScreen (
    IN UINT32 X,
    IN UINT32 Y,
    IN UINT32 Width,
    IN UINT32 Height,
    IN UINT32 Color
    );

VOID
GfxFillBuffer (
    IN CONST GFX_FRAME_BUFFER   *FrameBuffer,
    IN UINT32                   Color
    );

VOID
GfxFillScreen (
    IN UINT32 Color
    );

UINT32
GfxGetHorizontalResolution ();

UINT32
GfxGetVerticalResolution ();

VOID
GfxPlayDrawMemoryAsBitmapDemo ();

#endif
