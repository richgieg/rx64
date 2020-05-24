#include <efi.h>
#include <efilib.h>
#include <intrin.h>
#include "graphics.h"
#include "load.h"
#include "util.h"

EFI_STATUS
efi_main (
    EFI_HANDLE          ImageHandle,
    EFI_SYSTEM_TABLE    *SystemTable
    )
{
    EFI_STATUS          Status;
    GRAPHICS_INFO       *GraphicsInfo;
    KERNEL_IMAGE_INFO   *KernelImageInfo;

    InitializeLib(ImageHandle, SystemTable);

    Status = GetGraphicsInfo(&GraphicsInfo);
    if (EFI_ERROR(Status)) {
        Print(L"GetGraphicsInfo failed\n");
        return Status;
    }

    Print(L"FrameBufferBase: %lx\n", GraphicsInfo->FrameBufferBase);
    Print(L"Resolution:      %dx%d\n\n", GraphicsInfo->HorizontalResolution,
        GraphicsInfo->VerticalResolution);

    Status = LoadKernelImage(ImageHandle, L"kernel.exe", &KernelImageInfo);
    if (EFI_ERROR(Status)) {
        Print(L"LoadKernelImage failed\n");
        return Status;
    }

    WaitForKeyStroke(L"Press any key to enter kernel...");

    KernelImageInfo->kmain(GraphicsInfo->FrameBufferBase,
        GraphicsInfo->HorizontalResolution, GraphicsInfo->VerticalResolution);

    return EFI_SUCCESS;
}
