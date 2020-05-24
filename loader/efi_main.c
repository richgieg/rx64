#include <efi.h>
#include <efilib.h>
#include "util.h"

VOID
LoadKernelImage (
    IN EFI_HANDLE   LoaderImageHandle,
    IN CHAR16       *FileName
    );

EFI_STATUS
efi_main (
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
    )
{
    InitializeLib(ImageHandle, SystemTable);

    LoadKernelImage(ImageHandle, L"kernel.exe");

    WaitForKeyStroke(L"Press any key to exit...");

    return EFI_SUCCESS;
}

VOID
LoadKernelImage (
    IN EFI_HANDLE   LoaderImageHandle,
    IN CHAR16       *FileName
    )
{
    EFI_STATUS                  Status;
    EFI_LOADED_IMAGE            *LoadedImage;
    EFI_DEVICE_PATH             *DevicePath;
    EFI_FILE_IO_INTERFACE       *FileSystem;
    EFI_FILE_HANDLE             RootDirectory;
    EFI_FILE_HANDLE             KernelFileHandle;
    EFI_FILE_INFO               *FileInfo;
    UINTN                       FileInfoSize;
    UINTN                       KernelBufferSize;
    VOID                        *KernelBuffer;

    // Get info about this loader's image.
    Status = BS->HandleProtocol(
        LoaderImageHandle, &LoadedImageProtocol, (VOID **)&LoadedImage);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to get info about loader image\n");
        Exit(EFI_SUCCESS, 0, NULL);
    }

    // Get device path for this loader's image.
    Status = BS->HandleProtocol(
        LoadedImage->DeviceHandle, &DevicePathProtocol, (VOID **)&DevicePath);
    if (EFI_ERROR(Status) || DevicePath == NULL) {
        Print(L"Failed to get device path for loader image\n");
        Exit(EFI_SUCCESS, 0, NULL);
    }

    // Open the volume where this loader's image resides.
    Status = BS->HandleProtocol(
        LoadedImage->DeviceHandle, &FileSystemProtocol, (VOID **)&FileSystem);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to get handle to loader file system\n");
        Exit(EFI_SUCCESS, 0, NULL);
    }
    Status = FileSystem->OpenVolume(FileSystem, &RootDirectory);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to open loader file system volume\n");
        Exit(EFI_SUCCESS, 0, NULL);
    }

    // Open kernel image file.
    Status = RootDirectory->Open(
        RootDirectory, &KernelFileHandle, FileName, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to open kernel image\n");
        Exit(EFI_SUCCESS, 0, NULL);
    }

    // Get size of kernel image.
    FileInfoSize = 0;
    Status = KernelFileHandle->GetInfo(KernelFileHandle, &gEfiFileInfoGuid, &FileInfoSize, NULL);
    if (Status != EFI_BUFFER_TOO_SMALL) {
        Print(L"Failed to get required size to store kernel file info\n");
        Exit(EFI_SUCCESS, 0, NULL);
    }
    FileInfo = AllocatePool(FileInfoSize);
    Status = KernelFileHandle->GetInfo(KernelFileHandle, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to get kernel file info\n");
        Exit(EFI_SUCCESS, 0, NULL);
    }

    // Load kernel into memory.
    KernelBufferSize = FileInfo->FileSize;
    KernelBuffer = AllocatePool(KernelBufferSize);
    if (KernelBuffer == NULL) {
        Print(L"Failed to allocate memory for kernel image\n");
        Exit(EFI_SUCCESS, 0, NULL);
    }
    Status = KernelFileHandle->Read(KernelFileHandle, &KernelBufferSize, KernelBuffer);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to read kernel image");
        Exit(EFI_SUCCESS, 0, NULL);
    }
    Status = KernelFileHandle->Close(KernelFileHandle);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to close kernel image file handle\n");
        Exit(EFI_SUCCESS, 0, NULL);
    }

    Print(L"Kernel Image Size: %d bytes\n", KernelBufferSize);

}
