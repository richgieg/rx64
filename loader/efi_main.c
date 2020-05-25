#include <efi.h>
#include <efilib.h>
#include <intrin.h>
#include "graphics.h"
#include "load.h"
#include "memory.h"
#include "util.h"

EFI_STATUS
efi_main (
    EFI_HANDLE          ImageHandle,
    EFI_SYSTEM_TABLE    *SystemTable
    )
{
    EFI_STATUS              Status;
    VOID                    (*kmain)(LOADER_INFO *);
    LOADER_GRAPHICS_INFO    *GraphicsInfo;
    LOADER_MEMORY_INFO      *MemoryInfo;
    LOADER_INFO             *LoaderInfo;

    InitializeLib(ImageHandle, SystemTable);

    Status = LoadKernelImage(ImageHandle, L"kernel.exe", &kmain);
    if (EFI_ERROR(Status)) {
        Print(L"LoadKernelImage failed\n");
        return Status;
    }

    Status = GetGraphicsInfo(&GraphicsInfo);
    if (EFI_ERROR(Status)) {
        Print(L"GetGraphicsInfo failed\n");
        return Status;
    }

    Status = GetMemoryInfo(&MemoryInfo);
    if (EFI_ERROR(Status)) {
        Print(L"GetMemoryInfo failed\n");
        return Status;
    }

    LoaderInfo = AllocatePool(sizeof(LoaderInfo));
    LoaderInfo->GraphicsInfo = GraphicsInfo;
    LoaderInfo->MemoryInfo = MemoryInfo;

    WaitForKeyStroke(L"Press any key to enter kernel...");

    // Transfer control to the kernel.
    kmain(LoaderInfo);

    // Kernel should never return, but in case it does...
    for (;;) {
        __halt();
    }

    return EFI_SUCCESS;
}
