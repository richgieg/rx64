#include <efi.h>
#include <efilib.h>
#include "load.h"
#include "memory.h"

EFI_STATUS
LoadKernelImage (
    IN EFI_HANDLE           LoaderImageHandle,
    IN CHAR16               *FileName,
    OUT KERNEL_IMAGE_INFO   **KernelImageInfo
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
    UINT8                       *KernelBuffer;
    IMAGE_DOS_HEADER            *DosHeader;
    IMAGE_NT_HEADERS64          *NtHeader;
    IMAGE_SECTION_HEADER        *SectionHeader;
    UINT8                       *SectionData;
    EFI_PHYSICAL_ADDRESS        PhysicalAddress;
    EFI_VIRTUAL_ADDRESS         VirtualAddress;
    UINTN NumPages;

    // Get info about this loader's image.
    Status = BS->HandleProtocol(
        LoaderImageHandle, &LoadedImageProtocol, (VOID **)&LoadedImage);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to get info about loader image\n");
        return Status;
    }

    // Get device path for this loader's image.
    Status = BS->HandleProtocol(
        LoadedImage->DeviceHandle, &DevicePathProtocol, (VOID **)&DevicePath);
    if (EFI_ERROR(Status) || DevicePath == NULL) {
        Print(L"Failed to get device path for loader image\n");
        return Status;
    }

    // Open the volume where this loader's image resides.
    Status = BS->HandleProtocol(
        LoadedImage->DeviceHandle, &FileSystemProtocol, (VOID **)&FileSystem);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to get handle to loader file system\n");
        return Status;
    }
    Status = FileSystem->OpenVolume(FileSystem, &RootDirectory);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to open loader file system volume\n");
        return Status;
    }

    // Open kernel image file.
    Status = RootDirectory->Open(
        RootDirectory, &KernelFileHandle, FileName, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to open kernel image\n");
        return Status;
    }

    // Get size of kernel image.
    FileInfoSize = 0;
    Status = KernelFileHandle->GetInfo(KernelFileHandle, &gEfiFileInfoGuid, &FileInfoSize, NULL);
    if (Status != EFI_BUFFER_TOO_SMALL) {
        Print(L"Failed to get required size to store kernel file info\n");
        return Status;
    }
    FileInfo = AllocatePool(FileInfoSize);
    Status = KernelFileHandle->GetInfo(KernelFileHandle, &gEfiFileInfoGuid, &FileInfoSize, FileInfo);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to get kernel file info\n");
        return Status;
    }

    // Load kernel into memory.
    KernelBufferSize = FileInfo->FileSize;
    KernelBuffer = AllocatePool(KernelBufferSize);
    if (KernelBuffer == NULL) {
        Print(L"Failed to allocate memory for kernel image\n");
        return Status;
    }
    Status = KernelFileHandle->Read(KernelFileHandle, &KernelBufferSize, KernelBuffer);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to read kernel image");
        return Status;
    }
    Status = KernelFileHandle->Close(KernelFileHandle);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to close kernel image file handle\n");
        return Status;
    }

    DosHeader = (IMAGE_DOS_HEADER *)KernelBuffer;
    NtHeader = (IMAGE_NT_HEADERS64 *)((UINT64)DosHeader + DosHeader->e_lfanew);
    SectionHeader = (IMAGE_SECTION_HEADER *)(
        (UINT8 *)(&NtHeader->OptionalHeader) + NtHeader->FileHeader.SizeOfOptionalHeader);

    NumPages = NtHeader->OptionalHeader.SizeOfHeaders / EFI_PAGE_SIZE;
    if (NtHeader->OptionalHeader.SizeOfHeaders % EFI_PAGE_SIZE) {
        NumPages++;
    }
    Status = BS->AllocatePages(AllocateAnyPages, EfiLoaderData, NumPages, &PhysicalAddress);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to allocated pages for kernel headers\n");
        return Status;
    }
    ZeroMem((VOID *)PhysicalAddress, NumPages * EFI_PAGE_SIZE);
    CopyMem((VOID *)PhysicalAddress, KernelBuffer,
        NtHeader->OptionalHeader.SizeOfHeaders);

    SectionData = KernelBuffer + NtHeader->OptionalHeader.SizeOfHeaders;

    for (int i = 0; i < NtHeader->FileHeader.NumberOfSections; i++) {
        NumPages = SectionHeader[i].Misc.VirtualSize / EFI_PAGE_SIZE;
        if (SectionHeader[i].Misc.VirtualSize % EFI_PAGE_SIZE) {
            NumPages++;
        }
        Status = BS->AllocatePages(AllocateAnyPages, EfiLoaderData, NumPages, &PhysicalAddress);
        if (EFI_ERROR(Status)) {
            Print(L"Failed to allocate pages for kernel %a section\n", SectionHeader->Name);
            return Status;
        }
        ZeroMem((VOID *)PhysicalAddress, NumPages * EFI_PAGE_SIZE);
        CopyMem((VOID *)PhysicalAddress, SectionData, SectionHeader[i].SizeOfRawData);

        VirtualAddress = NtHeader->OptionalHeader.ImageBase +
            SectionHeader[i].VirtualAddress;
        MapVirtualToPhysicalPages(VirtualAddress, PhysicalAddress, NumPages);

        Print(L"%a\n", SectionHeader[i].Name);
        Print(L"    Virtual Address:  %lx\n", VirtualAddress);
        Print(L"    Virtual Size:     %lx\n", SectionHeader[i].Misc.VirtualSize);
        Print(L"    Size of Raw Data: %lx\n", SectionHeader[i].SizeOfRawData);
        Print(L"    Number of Pages:  %d\n", NumPages);
        Print(L"\n");

        SectionData += SectionHeader[i].SizeOfRawData;
    }

    *KernelImageInfo = AllocatePool(sizeof(KERNEL_IMAGE_INFO));
    (*KernelImageInfo)->kmain = (VOID *)(NtHeader->OptionalHeader.ImageBase +
        NtHeader->OptionalHeader.AddressOfEntryPoint);

    return EFI_SUCCESS;
}
