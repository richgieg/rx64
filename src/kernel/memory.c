// Very naive memory manager... will be improved!

#include "kernel.h"
#include "memory.h"

#define PAGE_BASE_ADDRESS 0x700000;
#define POOL_BASE_ADDRESS 0x6f0000;

static UINTN mNextPageAddress;
static UINTN mNextPoolAddress;

VOID
MmInitialize ()
{
    mNextPageAddress = PAGE_BASE_ADDRESS;
    mNextPoolAddress = POOL_BASE_ADDRESS;
}

VOID *
MmAllocatePages (
    UINTN NoPages
    )
{
    VOID *Address;
    Address = (VOID *)mNextPageAddress;
    mNextPageAddress += NoPages * MM_PAGE_SIZE;
    return Address;
}

VOID *
MmAllocatePool (
    UINTN Size
    )
{
    // Temporary... Make sure pool allocation doesn't
    // run into page allocations.
    if (mNextPoolAddress + Size >= 0x700000) {
        KeBugCheck();
    }
    VOID *Address;
    Address = (VOID *)mNextPoolAddress;
    mNextPageAddress += Size;
    return Address;
}
