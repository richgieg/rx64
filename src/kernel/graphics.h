#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "types.h"

VOID
InitializeGraphics (
    UINT64 FrameBufferBase,
    UINT64 FrameBufferSize,
    UINT32 HorizontalResolution,
    UINT32 VerticalResolution
    );

VOID
FillBlock (
    UINT32 X,
    UINT32 Y,
    UINT32 Width,
    UINT32 Height,
    UINT32 Color
    );

VOID
FillScreen (
    UINT32 Color
    );

#endif
