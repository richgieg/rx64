#include <efi.h>
#include <efilib.h>

VOID
PrintMemoryMap (
    IN EFI_MEMORY_DESCRIPTOR   *MemoryMap,
    IN UINTN                   NoEntries,
    IN UINTN                   DescriptorSize,
    IN BOOLEAN                 PrintDescriptors
    );

VOID
WaitForKeyStroke ();
