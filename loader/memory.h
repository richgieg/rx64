#pragma once

#include <efi.h>
#include "../kernel/loader_info.h"

#define PagesFromBytes(b) ((UINTN)(b / EFI_PAGE_SIZE) + ((b % EFI_PAGE_SIZE) ? 1 : 0))

EFI_STATUS
GetMemoryInfo (
    OUT LOADER_MEMORY_INFO  **MemoryInfo,
    OUT UINTN               *MemoryMapKey
    );

EFI_STATUS
MapMemory (
    IN LOADER_MEMORY_MAPPING_TYPE   Type,
    IN UINT64                       PhysicalAddress,
    IN UINT64                       VirtualAddress,
    IN UINTN                        NumPages
    );
