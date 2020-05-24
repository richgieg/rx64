#include <efi.h>
#include <efilib.h>
#include <intrin.h>
#include "load.h"
#include "util.h"

EFI_STATUS
efi_main (
    EFI_HANDLE          ImageHandle,
    EFI_SYSTEM_TABLE    *SystemTable
    )
{
    EFI_STATUS          Status;
    KERNEL_IMAGE_INFO   *KernelImageInfo;

    InitializeLib(ImageHandle, SystemTable);

    Status = LoadKernelImage(ImageHandle, L"kernel.exe", &KernelImageInfo);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to load kernel image\n");
        return Status;
    }

    Print(L"%lx\n", KernelImageInfo->kmain);

    WaitForKeyStroke(L"Press any key to enter kernel...");

    KernelImageInfo->kmain();

    return EFI_SUCCESS;
}
