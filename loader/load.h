#pragma once

#include <efi.h>
#include "../kernel/kmain.h"

EFI_STATUS
LoadKernelImage (
    IN EFI_HANDLE           LoaderImageHandle,
    IN CHAR16               *FileName,
    OUT VOID                (**kmain)(LOADER_INFO *)
    );
