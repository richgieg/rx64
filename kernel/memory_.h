#pragma once

#include <loader_info.h>
#include <types.h>

#define MM_PRIVATE_POOL_VIRTUAL_ADDRESS     0xffff808000000000;

typedef struct _MM_PAGE_ALLOCATION {
    UINT64                      PhysicalAddress;
    UINT64                      VirtualAddress;
    UINT64                      NumPages;
    struct _MM_PAGE_ALLOCATION  *NextPhysical;
    struct _MM_PAGE_ALLOCATION  *NextVirtual;
} MM_PAGE_ALLOCATION;

VOID
MmInitializeMemory (
    IN LOADER_MEMORY_INFO *MemoryInfo
    );

VOID *
MmAllocateInitPool (
    IN UINT64 Size
    );

VOID *
MmAllocatePhysicalPages (
    IN UINT64 NumPages
    );

#ifdef _DEBUG

VOID
PrintLoaderMemoryInfo (
    IN LOADER_MEMORY_INFO *MemoryInfo
    );

VOID
PrintPhysicalAllocations ();

VOID
PrintUsableRanges ();

#endif
