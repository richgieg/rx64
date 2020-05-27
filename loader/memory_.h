#pragma once

#include <efi.h>

EFI_STATUS
MapPage (
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress
    );
