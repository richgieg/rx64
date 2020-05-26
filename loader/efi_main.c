#include <efi.h>
#include <efilib.h>
#include <intrin.h>
#include "graphics.h"
#include "load.h"
#include "memory.h"
#include "util.h"
#include "../kernel/loader_info.h"

EFI_STATUS
efi_main (
    EFI_HANDLE          ImageHandle,
    EFI_SYSTEM_TABLE    *SystemTable
    )
{
    EFI_STATUS              Status;
    LOADER_INFO             *LoaderInfo;
    KERNEL_ENTRY            KernelEntry;
    LOADER_GRAPHICS_INFO    *GraphicsInfo;
    LOADER_MEMORY_INFO      *MemoryInfo;
    UINTN                   MemoryMapKey;

    InitializeLib(ImageHandle, SystemTable);
    LoaderInfo = AllocatePool(sizeof(LoaderInfo));

    Status = LoadKernelImage(ImageHandle, L"kernel.exe", &KernelEntry);
    if (EFI_ERROR(Status)) {
        Print(L"LoadKernelImage failed\n");
        WaitForKeyStroke(L"Press any key to exit loader\n");
        return Status;
    }

    Status = GetGraphicsInfo(&GraphicsInfo);
    if (EFI_ERROR(Status)) {
        Print(L"GetGraphicsInfo failed\n");
        WaitForKeyStroke(L"Press any key to exit loader\n");
        return Status;
    }
    LoaderInfo->GraphicsInfo = GraphicsInfo;

    Status = GetMemoryInfo(&MemoryInfo, &MemoryMapKey);
    if (EFI_ERROR(Status)) {
        Print(L"GetMemoryInfo failed\n");
        WaitForKeyStroke(L"Press any key to exit loader\n");
        return Status;
    }
    LoaderInfo->MemoryInfo = MemoryInfo;

    Status = BS->ExitBootServices(ImageHandle, MemoryMapKey);
    if (EFI_ERROR(Status)) {
        Print(L"ExitBootServices failed\n");
        WaitForKeyStroke(L"Press any key to exit loader\n");
        return Status;
    }
    KernelEntry(LoaderInfo);

    // Kernel should never return, but in case it does...
    for (;;) {
        __halt();
    }

    return EFI_SUCCESS;
}
