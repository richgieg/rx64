#ifndef RECON_H_
#define RECON_H_

#include <efi.h>

VOID
PrintPml4Table (
    IN BOOLEAN SkipLargePages,
    IN BOOLEAN PauseAfterEntry
    );

VOID
PrintPdpTable (
    IN EFI_PHYSICAL_ADDRESS     PdpTableAddress,
    IN BOOLEAN                  SkipLargePages,
    IN BOOLEAN                  PauseAfterEntry
    );

VOID
PrintPageDirectory (
    IN EFI_PHYSICAL_ADDRESS     PageDirectoryAddress,
    IN BOOLEAN                  SkipLargePages,
    IN BOOLEAN                  PauseAfterEntry
    );

VOID
PrintPageTable (
    IN EFI_PHYSICAL_ADDRESS     PageTableAddress,
    IN BOOLEAN                  PauseAfterEntry
    );

VOID
PrintControlRegisters ();

VOID
PrintEnvironmentVariables (
    IN BOOLEAN PauseAfterEntry
    );

VOID
PrintMemoryMap (
    IN BOOLEAN PauseAfterEntry
    );

#endif
