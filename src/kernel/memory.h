#ifndef MEMORY_H_
#define MEMORY_H_

#include "types.h"

#define MM_PAGE_SIZE 4096

VOID
MmInitialize (
    UINT64  ContiguousPhysicalMemoryBase,
    UINTN   NoPages
    );

VOID *
MmAllocatePages (
    UINTN NoPages
    );

VOID *
MmAllocatePool (
    UINTN Size
    );

UINT64
MmGetPml4TableAddress ();

VOID
MmMapVirtualToPhysicalPages (
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress,
    IN UINTN    NoPages
    );

VOID
MmMapVirtualToPhysicalPage (
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress
    );

#endif
