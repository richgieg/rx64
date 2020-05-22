// Very naive memory manager... will be improved!

#include "kernel.h"
#include "memory.h"

static UINT64   mContiguousPhysicalMemoryBase;
static UINTN    mNoPages;
static UINT64   mBasePoolAddress;
static UINT64   mNextPoolAddress;
static UINT64   mBasePageAddress;
static UINT64   mNextPageAddress;

VOID
MmInitialize (
    UINT64  ContiguousPhysicalMemoryBase,
    UINTN   NoPages
    )
{
    mContiguousPhysicalMemoryBase = ContiguousPhysicalMemoryBase;
    mNoPages = NoPages;
    // Pool is 1 MB.
    mBasePoolAddress = mContiguousPhysicalMemoryBase;
    mNextPoolAddress = mBasePoolAddress;
    // Start page allocations after the pool.
    mBasePageAddress = mBasePoolAddress + 0x100000;
    mNextPageAddress = mBasePageAddress;
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
    // Ensure that new pool allocation doesn't run into page allocations.
    if (mNextPoolAddress + Size >= mBasePageAddress) {
        KeBugCheck();
    }
    VOID *Address;
    Address = (VOID *)mNextPoolAddress;
    mNextPoolAddress += Size;
    return Address;
}
