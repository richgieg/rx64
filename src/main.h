#include <efi.h>
#include <efilib.h>

EFI_STATUS
GetMemoryMap (
    OUT EFI_MEMORY_DESCRIPTOR   **MemoryMap,
    OUT UINTN                   *MapSize,
    OUT UINTN                   *DescriptorSize,
    OUT UINTN                   *MapKey
    );

VOID
PrintMemoryMap (
    IN EFI_MEMORY_DESCRIPTOR   *MemoryMap,
    IN UINTN                   MapSize,
    IN UINTN                   DescriptorSize,
    IN BOOLEAN                 PrintDescriptors
    );

VOID
WaitForKeyStroke ();
