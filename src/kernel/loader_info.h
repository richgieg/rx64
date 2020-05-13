#ifndef LOADER_INFO_H_
#define LOADER_INFO_H_

#include "types.h"

typedef struct _LOADER_INFO_GRAPHICS {
    UINTN   FrameBufferBase;
    UINTN   FrameBufferSize;
    UINT32  HorizontalResolution;
    UINT32  VerticalResolution;
} LOADER_INFO_GRAPHICS;

typedef struct _LOADER_INFO {
    LOADER_INFO_GRAPHICS        Graphics;
} LOADER_INFO;

#endif
