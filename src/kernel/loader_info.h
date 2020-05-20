#ifndef LOADER_INFO_H_
#define LOADER_INFO_H_

#include "types.h"

typedef struct _LOADER_INFO {
    UINT64  KernelPhysicalAddress;
    UINTN   NoKernelPages;
    UINT64  FrameBufferBase;
    UINTN   FrameBufferSize;
    UINT32  HorizontalResolution;
    UINT32  VerticalResolution;
} LOADER_INFO;

#endif
