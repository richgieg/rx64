#ifndef MEMORY_H_
#define MEMORY_H_

#include "types.h"

#define MM_PAGE_SIZE 4096

VOID *
MmAllocatePages (
    UINTN NoPages
    );

#endif