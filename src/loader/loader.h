#ifndef LOADER_H_
#define LOADER_H_

#include <efi.h>
#include "../shared/loader_info.h"

VOID *
LoadKernelImage (
    IN EFI_HANDLE   LoaderImageHandle
    );

VOID
PrintEnvironmentVariables (
    IN BOOLEAN PauseAfterEntry
    );

VOID
PrintMemoryMap (
    IN EFI_MEMORY_DESCRIPTOR    *MemoryMap,
    IN UINTN                    NoEntries,
    IN UINTN                    DescriptorSize,
    IN BOOLEAN                  PauseAfterEntry
    );

VOID
SetGraphicsMode (
    LOADER_INFO_GRAPHICS *LoaderInfoForGraphics
    );

VOID
WaitForKeyStroke (
    IN CHAR16 *Prompt
    );

#endif
