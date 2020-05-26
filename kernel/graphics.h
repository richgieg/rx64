#pragma once

#include <types.h>

typedef struct _GFX_FRAME_BUFFER {
    UINT64 Base;
    UINT64 Size;
    UINT32 HorizontalResolution;
    UINT32 VerticalResolution;
} GFX_FRAME_BUFFER;

VOID
GfxBltLinesInBuffer (
    IN CONST GFX_FRAME_BUFFER   *Buffer,
    IN UINT32                   DestinationY,
    IN UINT32                   SourceY,
    IN UINT64                   NoLines
    );

VOID
GfxBltLinesOnScreen (
    IN UINT32                   DestinationY,
    IN UINT32                   SourceY,
    IN UINT64                   NoLines
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

UINT32
GfxGetHorizontalResolution ();

UINT32
GfxGetVerticalResolution ();
