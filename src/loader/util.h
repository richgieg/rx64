#ifndef UTIL_H_
#define UTIL_H_

#include <efi.h>
#include "../kernel/loader_info.h"

typedef struct _KERNEL_IMAGE_INFO {
    VOID    (*KernelEntry)(LOADER_INFO *LoaderInfo);
    UINT64  PhysicalAddress;
    UINTN   NoPages;
} KERNEL_IMAGE_INFO;

VOID
ExitBootServices (
    IN EFI_HANDLE LoaderImageHandle
    );

EFI_PHYSICAL_ADDRESS
GetPml4TableAddress ();

VOID
LoadKernelImage (
    IN EFI_HANDLE           LoaderImageHandle,
    IN CHAR16               *FileName,
    IN KERNEL_IMAGE_INFO    *Info
    );

VOID
WaitForKeyStroke (
    IN CHAR16   *Prompt
    );

#endif
