// kmain.h
// Included only by kmain.c and various modules in ../loader.

#pragma once

#include <types.h>

typedef struct _LOADER_GRAPHICS_INFO {
    UINT64 FrameBufferBase;
    UINT64 NumPages;
    UINT32 HorizontalResolution;
    UINT32 VerticalResolution;
} LOADER_GRAPHICS_INFO;

typedef struct _LOADER_MEMORY_MAPPING {
    UINT64 PhysicalAddress;
    UINT64 VirtualAddress;
    UINT64 NumPages;
} LOADER_MEMORY_MAPPING;

typedef struct _LOADER_AVAILABLE_MEMORY_RANGE {
    UINT64 PhysicalAddress;
    UINT64 NumPages;
} LOADER_AVAILABLE_MEMORY_RANGE;

typedef struct _LOADER_MEMORY_INFO {
    LOADER_MEMORY_MAPPING           *Mappings;
    UINT64                          NumMappings;
    LOADER_AVAILABLE_MEMORY_RANGE   *AvailableRanges;
    UINT64                          NumAvailableRanges;
} LOADER_MEMORY_INFO;

typedef struct _LOADER_INFO {
    LOADER_GRAPHICS_INFO    *GraphicsInfo;
    LOADER_MEMORY_INFO      *MemoryInfo;
} LOADER_INFO;

typedef void (*KERNEL_ENTRY)(LOADER_INFO *);
