#include <efi.h>
#include <efilib.h>
#include "graphics.h"
#include "intrin.h"
#include "load.h"
#include "memory.h"
#include "util.h"
#include "../kernel/loader_info.h"

VOID
ReportLoaderError (
    CHAR16 *Message
    );

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
        ReportLoaderError(L"LoadKernelImage failed.");
        return Status;
    }

    Status = GetGraphicsInfo(&GraphicsInfo);
    if (EFI_ERROR(Status)) {
        ReportLoaderError(L"GetGraphicsInfo failed.");
        return Status;
    }
    LoaderInfo->GraphicsInfo = GraphicsInfo;

    Status = GetMemoryInfo(&MemoryInfo, &MemoryMapKey);
    if (EFI_ERROR(Status)) {
        ReportLoaderError(L"GetMemoryInfo failed.");
        return Status;
    }
    LoaderInfo->MemoryInfo = MemoryInfo;

    Status = BS->ExitBootServices(ImageHandle, MemoryMapKey);
    if (EFI_ERROR(Status)) {
        ReportLoaderError(L"ExitBootServices failed.");
        return Status;
    }

    // Transfer control to kernel.
    KernelEntry(LoaderInfo);

    // Kernel should never return, but in case it does...
    for (;;) {
        __halt();
    }

    return EFI_SUCCESS;
}

VOID
ReportLoaderError (
    CHAR16 *Message
    )
{
    Print(L"%s\n\nPress any key to exit loader...\n", Message);
    WaitForKeyStroke(NULL);
}
