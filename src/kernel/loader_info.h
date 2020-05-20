#ifndef LOADER_INFO_H_
#define LOADER_INFO_H_

#include "types.h"

typedef struct _KERNEL_SECTION {
    UINT64  PhysicalAddress;
    UINT64  VirtualAddress;
    UINTN   NoPages;
} KERNEL_SECTION;

typedef struct _LOADER_INFO {
    UINTN           NoSections;
    KERNEL_SECTION  *Sections;
    UINT64          FrameBufferBase;
    UINTN           FrameBufferSize;
    UINT32          HorizontalResolution;
    UINT32          VerticalResolution;
} LOADER_INFO;

#endif
