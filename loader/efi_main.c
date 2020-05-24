#include <efi.h>
#include <efilib.h>
#include <intrin.h>
#include "util.h"

EFI_STATUS
efi_main (
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
    )
{
    InitializeLib(ImageHandle, SystemTable);

    WaitForKeyStroke(L"Press any key to exit...");

    return EFI_SUCCESS;
}
