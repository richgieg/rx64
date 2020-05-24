#pragma once

#include <efi.h>

VOID
MapVirtualToPhysicalPages (
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress,
    IN UINTN    NumPages
    );

VOID
MapVirtualToPhysicalPage (
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress
    );
