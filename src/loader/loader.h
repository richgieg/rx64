#include <efi.h>

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
    EFI_PHYSICAL_ADDRESS    *FrameBufferBase,
    UINTN                   *FrameBufferSize
    );

VOID
WaitForKeyStroke (
    IN CHAR16 *Prompt
    );
