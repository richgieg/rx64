#pragma once

#include <types.h>

typedef struct _GFX_FRAME_BUFFER {
    UINT64 Base;
    UINT64 Size;
    UINT32 HorizontalResolution;
    UINT32 VerticalResolution;
} GFX_FRAME_BUFFER;

VOID
GfxInitializeGraphics (
    IN UINT64 FrameBufferBase,
    IN UINT32 HorizontalResolution,
    IN UINT32 VerticalResolution
    );

VOID
GfxFillBuffer (
    IN CONST GFX_FRAME_BUFFER   *Buffer,
    IN UINT32                   Color
    );

VOID
GfxFillScreen (
    IN UINT32 Color
    );
