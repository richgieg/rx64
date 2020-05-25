// kmain.h
// Included only by kmain.c and various modules in ../loader.

#pragma once

typedef struct _LOADER_GRAPHICS_INFO {
    unsigned __int64 FrameBufferBase;
    unsigned __int64 NumPages;
    unsigned __int32 HorizontalResolution;
    unsigned __int32 VerticalResolution;
} LOADER_GRAPHICS_INFO;

typedef struct _LOADER_MEMORY_MAPPING {
    unsigned __int64 PhysicalAddress;
    unsigned __int64 VirtualAddress;
    unsigned __int64 NumPages;
} LOADER_MEMORY_MAPPING;

typedef struct _LOADER_MEMORY_INFO {
    LOADER_MEMORY_MAPPING   *Mappings;
    unsigned __int64        NumMappings;
} LOADER_MEMORY_INFO;

typedef struct _LOADER_INFO {
    LOADER_GRAPHICS_INFO    *GraphicsInfo;
    LOADER_MEMORY_INFO      *MemoryInfo;
} LOADER_INFO;
