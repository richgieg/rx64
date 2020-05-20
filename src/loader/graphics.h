#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <efi.h>
#include "../kernel/loader_info.h"

typedef struct _SET_GRAPHICS_MODE_RESULT {
    UINT64  FrameBufferBase;
    UINTN   FrameBufferSize;
    UINT32  HorizontalResolution;
    UINT32  VerticalResolution;
} SET_GRAPHICS_MODE_RESULT;

VOID
SetGraphicsMode (
    IN SET_GRAPHICS_MODE_RESULT *Result
    );

#endif
