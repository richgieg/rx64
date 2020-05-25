#pragma once

#include <efi.h>
#include "../kernel/kmain.h"

#define PagesFromBytes(b) ((UINTN)(b / EFI_PAGE_SIZE) + ((b % EFI_PAGE_SIZE) ? 1 : 0))

EFI_STATUS
GetMemoryInfo (
    OUT LOADER_MEMORY_INFO **MemoryInfo
    );

EFI_STATUS
MapMemory (
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress,
    IN UINTN    NumPages
    );
