#pragma once

#include <loader_info.h>
#include <types.h>

typedef struct _MM_USABLE_MEMORY_RANGE {
    UINT64 PhysicalAddress;
    UINT64 NumPages;
} MM_USABLE_MEMORY_RANGE;

VOID
MmInitializeMemory (
    IN LOADER_MEMORY_INFO *MemoryInfo
    );

#ifdef _DEBUG

VOID
PrintLoaderMemoryInfo (
    IN LOADER_MEMORY_INFO *MemoryInfo
    );

#endif
