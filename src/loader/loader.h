#include <efi.h>

VOID *
LoadKernelImage (
    IN EFI_HANDLE   LoaderImageHandle
    );

VOID
PrintMemoryMap (
    IN EFI_MEMORY_DESCRIPTOR    *MemoryMap,
    IN UINTN                    NoEntries,
    IN UINTN                    DescriptorSize,
    IN BOOLEAN                  PauseAfterEntry
    );

VOID
PrintEnvironmentVariables (
    IN BOOLEAN PauseAfterEntry
    );

VOID
WaitForKeyStroke (
    IN CHAR16 *Prompt
    );
