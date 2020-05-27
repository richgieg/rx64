#pragma once

#include <loader_info.h>
#include <types.h>

VOID
MmInitializeMemory (
    LOADER_MEMORY_INFO *MemoryInfo
    );

VOID
PrintLoaderMemoryInfo (
    LOADER_MEMORY_INFO *MemoryInfo
    );
