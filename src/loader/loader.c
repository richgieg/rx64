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


    UINT64 CR0 = 0;
    UINT64 CR3 = 0;
    UINT64 CR4 = 0;
    asm(
        "mov %%cr0, %%rax;"
        "mov %%rax, %0;"
        "mov %%cr3, %%rax;"
        "mov %%rax, %1;"
        "mov %%cr4, %%rax;"
        "mov %%rax, %2;"

        :"=r"(CR0),
         "=r"(CR3),
         "=r"(CR4)
        :
        :"%rax"
    );

    Print(L"CR0: %x\n", CR0);
    Print(L"CR3: %x\n", CR3);
    Print(L"CR4: %x\n", CR4);


    // Kernel should never return, but if it does...
    for (;;) {
        asm("hlt");
    }
}
