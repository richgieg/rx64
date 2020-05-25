#pragma once

#include <efi.h>

VOID
MapPage (
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress
    );
