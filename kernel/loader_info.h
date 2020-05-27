#pragma once

#include <types.h>

typedef struct _LOADER_GRAPHICS_INFO {
    UINT64 FrameBufferBase;
    UINT32 HorizontalResolution;
    UINT32 VerticalResolution;
} LOADER_GRAPHICS_INFO;

typedef enum {
    LoaderKernelMemoryMapping,
    LoaderFrameBufferMemoryMapping,
    LoaderMaxMemoryMappingType
} LOADER_MEMORY_MAPPING_TYPE;

typedef struct _LOADER_MEMORY_MAPPING {
    LOADER_MEMORY_MAPPING_TYPE  Type;
    UINT64                      PhysicalAddress;
    UINT64                      VirtualAddress;
    UINT64                      NumPages;
} LOADER_MEMORY_MAPPING;

typedef struct _LOADER_USABLE_MEMORY_RANGE {
    UINT64 PhysicalAddress;
    UINT64 NumPages;
} LOADER_USABLE_MEMORY_RANGE;

typedef struct _LOADER_MEMORY_INFO {
    LOADER_MEMORY_MAPPING           *Mappings;
    UINT64                          NumMappings;
    LOADER_USABLE_MEMORY_RANGE      *UsableRanges;
    UINT64                          NumUsableRanges;
    UINT64                          ReservedRangePhysicalAddress;
    UINT64                          NumPagesInReservedRange;
} LOADER_MEMORY_INFO;

typedef struct _LOADER_INFO {
    LOADER_GRAPHICS_INFO    *GraphicsInfo;
    LOADER_MEMORY_INFO      *MemoryInfo;
} LOADER_INFO;

typedef void (*KERNEL_ENTRY) (
    IN LOADER_INFO *LoaderInfo
    );
