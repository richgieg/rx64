#pragma once

#include <efi.h>

typedef struct _GRAPHICS_INFO {
    UINT64  FrameBufferBase;
    UINTN   NumPages;
    UINT32  HorizontalResolution;
    UINT32  VerticalResolution;
} GRAPHICS_INFO;

EFI_STATUS
GetGraphicsInfo (
    GRAPHICS_INFO **GraphicsInfo
    );
