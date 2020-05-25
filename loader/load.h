#pragma once

#include <efi.h>

typedef struct _KERNEL_IMAGE_INFO {
    VOID (*kmain) (
        IN UINT64 FrameBufferBase,
        IN UINT32 HorizontalResolution,
        IN UINT32 VerticalResolution
        );
} KERNEL_IMAGE_INFO;

EFI_STATUS
LoadKernelImage (
    IN EFI_HANDLE           LoaderImageHandle,
    IN CHAR16               *FileName,
    OUT KERNEL_IMAGE_INFO   **KernelImageInfo
    );
