#pragma once

#include <efi.h>

typedef struct _MEMORY_MAPPING {
    UINT64  PhysicalAddress;
    UINT64  VirtualAddress;
    UINTN   NumPages;
} MEMORY_MAPPING;

UINTN
CalculatePagesFromBytes (
    IN UINTN Bytes
    );

UINTN
GetMemoryMappings (
    OUT MEMORY_MAPPING **MemoryMappings
    );

EFI_STATUS
MapMemory (
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress,
    IN UINTN    NumPages
    );
