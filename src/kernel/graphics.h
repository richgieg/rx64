#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "types.h"

VOID
InitializeGraphics (
    UINT64 FrameBufferBase,
    UINT64 FrameBufferSize
    );

VOID
FillScreen (
    UINT32 Color
    );

VOID
FillChar (
    UINT16 Column,
    UINT16 Row,
    UINT32 Color
    );

#endif
