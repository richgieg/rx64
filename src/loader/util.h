#ifndef UTIL_H_
#define UTIL_H_

#include <efi.h>
#include "../kernel/loader_info.h"

typedef struct _KERNEL_IMAGE_INFO {
    VOID                        (*KernelEntry)(LOADER_INFO *LoaderInfo);
    UINTN                       NoSectionMappings;
    LOADER_PAGE_MAPPING_INFO    SectionMappings[];
} KERNEL_IMAGE_INFO;

typedef struct _CONTIGUOUS_MEMORY_INFO {
    UINT64  BaseAddress;
    UINTN   NoPages;
} CONTIGUOUS_MEMORY_INFO;

VOID
ExitBootServices (
    IN EFI_HANDLE               LoaderImageHandle,
    IN CONTIGUOUS_MEMORY_INFO   *ContiguousPhysicalMemoryInfo
    );

EFI_PHYSICAL_ADDRESS
GetPml4TableAddress ();

KERNEL_IMAGE_INFO *
LoadKernelImage (
    IN EFI_HANDLE   LoaderImageHandle,
    IN CHAR16       *FileName
    );

VOID
MapVirtualToPhysicalPages (
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress,
    IN UINTN    NoPages
    );

VOID
MapVirtualToPhysicalPage (
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress
    );

VOID
WaitForKeyStroke (
    IN CHAR16   *Prompt
    );

#endif
