#ifndef UTIL_H_
#define UTIL_H_

#include <efi.h>

VOID
ExitBootServices (
    IN EFI_HANDLE LoaderImageHandle
    );

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
