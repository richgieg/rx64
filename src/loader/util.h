#ifndef UTIL_H_
#define UTIL_H_

#include <efi.h>

VOID
ExitBootServices (
    IN EFI_HANDLE LoaderImageHandle
    );

EFI_PHYSICAL_ADDRESS
GetPml4TableAddress ();

VOID *
LoadKernelImage (
    IN EFI_HANDLE   LoaderImageHandle,
    IN CHAR16       *FileName
    );

VOID
WaitForKeyStroke (
    IN CHAR16   *Prompt
    );

#endif
