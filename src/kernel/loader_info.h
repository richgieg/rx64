#ifndef LOADER_INFO_H_
#define LOADER_INFO_H_

#include "types.h"

typedef struct _LOADER_PAGE_MAPPING_INFO {
    UINT64  PhysicalAddress;
    UINT64  VirtualAddress;
    UINTN   NoPages;
} LOADER_PAGE_MAPPING_INFO;

typedef struct _LOADER_INFO {
    UINTN                       NoKernelSectionMappings;
    LOADER_PAGE_MAPPING_INFO    *KernelSectionMappings;
    UINT64                      FrameBufferPhysicalAddress;
    UINTN                       FrameBufferSize;
    UINT32                      HorizontalResolution;
    UINT32                      VerticalResolution;
} LOADER_INFO;

#endif
