#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "types.h"

VOID
GfxInitializeGraphics (
    UINT64 FrameBufferBase,
    UINT64 FrameBufferSize,
    UINT32 HorizontalResolution,
    UINT32 VerticalResolution
    );

VOID
GfxDrawBitmap (
    UINT32          X,
    UINT32          Y,
    UINT32          Width,
    UINT32          Height,
    UINT32          mForegroundColor,
    UINT32          mBackgroundColor,
    CONST UINT8     *SrcBuffer
    );

VOID
GfxFillBlock (
    UINT32 X,
    UINT32 Y,
    UINT32 Width,
    UINT32 Height,
    UINT32 Color
    );

VOID
GfxFillScreen (
    UINT32 Color
    );

UINT32
GfxGetHorizontalResolution ();

UINT32
GfxGetVerticalResolution ();

VOID
GfxPlayDrawMemoryAsBitmapDemo ();

#endif
