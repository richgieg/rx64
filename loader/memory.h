#pragma once

#include <efi.h>

UINTN
CalculatePagesFromBytes (
    IN UINTN Bytes
    );

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
