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
    KERNEL_IMAGE_INFO       *KernelImageInfo;
    LOADER_GRAPHICS_INFO    *GraphicsInfo;
    LOADER_MEMORY_INFO      *MemoryInfo;
    LOADER_INFO             *LoaderInfo;

    InitializeLib(ImageHandle, SystemTable);

    Status = LoadKernelImage(ImageHandle, L"kernel.exe", &KernelImageInfo);
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

    KernelImageInfo->kmain(GraphicsInfo->FrameBufferBase,
        GraphicsInfo->HorizontalResolution, GraphicsInfo->VerticalResolution);

    return EFI_SUCCESS;
}
