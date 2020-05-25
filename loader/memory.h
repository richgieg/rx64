#pragma once

#include <efi.h>
#include "../kernel/kmain.h"

UINTN
CalculatePagesFromBytes (
    IN UINTN Bytes
    );

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
