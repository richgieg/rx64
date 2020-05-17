#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include "graphics.h"
#include "recon.h"
#include "util.h"
#include "../kernel/loader_info.h"

EFI_STATUS
efi_main (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
    )
{
    LOADER_INFO     *LoaderInfo;
    VOID            (*KernelEntry)(LOADER_INFO *LoaderInfo);

    // Initialize EFI library (Set BS, RT, and ST globals).
    // BS = Boot Services, RT = Runtime Services, ST = System Table.
    InitializeLib(ImageHandle, SystemTable);
    // // PrintEnvironmentVariables(FALSE);
    // // PrintMemoryMap(FALSE);
    // KernelEntry = LoadKernelImage(ImageHandle, L"kernel.elf");
    // // WaitForKeyStroke(L"\nPress any key to launch kernel...\n");
    // LoaderInfo = AllocatePool(sizeof(LOADER_INFO));
    // SetGraphicsMode(&LoaderInfo->Graphics);
    // ExitBootServices(ImageHandle);
    // KernelEntry(LoaderInfo);

    PrintControlRegisters();

    // Kernel should never return, but if it does...
    for (;;) {
        asm("hlt");
    }
}
