#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "types.h"

typedef struct _GFX_FRAME_BUFFER {
    UINT64  Base;
    UINTN   Size;
    UINT32  HorizontalResolution;
    UINT32  VerticalResolution;
} GFX_FRAME_BUFFER;

VOID
GfxInitializeGraphics (
    IN UINT64 FrameBufferPhysicalAddress,
    IN UINTN  FrameBufferSize,
    IN UINT32 HorizontalResolution,
    IN UINT32 VerticalResolution
    );

GFX_FRAME_BUFFER *
GfxCreateBuffer ();

VOID
GfxBltLinesInBuffer (
    IN CONST GFX_FRAME_BUFFER   *Buffer,
    IN UINT32                   DestinationY,
    IN UINT32                   SourceY,
    IN UINTN                    NoLines
    );

VOID
GfxBltLinesOnScreen (
    IN UINT32                   DestinationY,
    IN UINT32                   SourceY,
    IN UINTN                    NoLines
    );

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
    );

VOID
GfxDrawBitmapOnScreen (
    IN UINT32                   X,
    IN UINT32                   Y,
    IN UINT32                   Width,
    IN UINT32                   Height,
    IN UINT32                   ForegroundColor,
    IN UINT32                   BackgroundColor,
    IN CONST UINT8              *Bitmap
    );

VOID
GfxFillBlockInBuffer (
    IN CONST GFX_FRAME_BUFFER   *Buffer,
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
    IN CONST GFX_FRAME_BUFFER   *Buffer,
    IN UINT32                   Color
    );

VOID
GfxFillScreen (
    IN UINT32 Color
    );

VOID
GfxCopyBuffer (
    IN CONST GFX_FRAME_BUFFER *DestinationBuffer,
    IN CONST GFX_FRAME_BUFFER *SourceBuffer
    );

VOID
GfxCopyBufferToScreen (
    IN CONST GFX_FRAME_BUFFER *Buffer
    );

UINT32
GfxGetHorizontalResolution ();

UINT32
GfxGetVerticalResolution ();

VOID
GfxDemo_DrawMemoryAsBitmap ();

#endif
