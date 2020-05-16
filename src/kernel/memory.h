#ifndef MEMORY_H_
#define MEMORY_H_

#include "types.h"

#define MM_PAGE_SIZE 4096

VOID
MmInitialize ();

VOID *
MmAllocatePages (
    UINTN NoPages
    );

VOID *
MmAllocatePool (
    UINTN Size
    );

#endif
