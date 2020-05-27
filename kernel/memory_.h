#pragma once

#include <loader_info.h>
#include <types.h>

VOID
MmInitializeMemory (
    IN LOADER_MEMORY_INFO *MemoryInfo
    );

VOID
PrintLoaderMemoryInfo (
    IN LOADER_MEMORY_INFO *MemoryInfo
    );
