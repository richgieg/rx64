#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "types.h"

VOID
GfxInitializeGraphics (
    IN UINT64 FrameBufferBase,
    IN UINT64 FrameBufferSize,
    IN UINT32 HorizontalResolution,
    IN UINT32 VerticalResolution
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
GfxFillBlock (
    IN UINT32 X,
    IN UINT32 Y,
    IN UINT32 Width,
    IN UINT32 Height,
    IN UINT32 Color
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
