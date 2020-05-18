#ifndef RECON_H_
#define RECON_H_

#include <efi.h>

VOID
PrintPml4Entries ();

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
