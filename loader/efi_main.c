#include <efi.h>
#include <efilib.h>
#include "util.h"

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16

typedef struct _IMAGE_FILE_HEADER {
    UINT16 Machine;
    UINT16 NumberOfSections;
    UINT32 TimeDateStamp;
    UINT32 PointerToSymbolTable;
    UINT32 NumberOfSymbols;
    UINT16 SizeOfOptionalHeader;
    UINT16 Characteristics;
} IMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY {
    UINT32 VirtualAddress;
    UINT32 Size;
} IMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_OPTIONAL_HEADER64 {
    UINT16                  Magic;
    UINT8                   MajorLinkerVersion;
    UINT8                   MinorLinkerVersion;
    UINT32                  SizeOfCode;
    UINT32                  SizeOfInitializedData;
    UINT32                  SizeOfUninitializedData;
    UINT32                  AddressOfEntryPoint;
    UINT32                  BaseOfCode;
    UINT64                  ImageBase;
    UINT32                  SectionAlignment;
    UINT32                  FileAlignment;
    UINT16                  MajorOperatingSystemVersion;
    UINT16                  MinorOperatingSystemVersion;
    UINT16                  MajorImageVersion;
    UINT16                  MinorImageVersion;
    UINT16                  MajorSubsystemVersion;
    UINT16                  MinorSubsystemVersion;
    UINT32                  Win32VersionValue;
    UINT32                  SizeOfImage;
    UINT32                  SizeOfHeaders;
    UINT32                  CheckSum;
    UINT16                  Subsystem;
    UINT16                  DllCharacteristics;
    UINT64                  SizeOfStackReserve;
    UINT64                  SizeOfStackCommit;
    UINT64                  SizeOfHeapReserve;
    UINT64                  SizeOfHeapCommit;
    UINT32                  LoaderFlags;
    UINT32                  NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY    DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER64;

typedef struct _IMAGE_NT_HEADERS64 {
    UINT32                      Signature;
    IMAGE_FILE_HEADER           FileHeader;
    IMAGE_OPTIONAL_HEADER64     OptionalHeader;
} IMAGE_NT_HEADERS64;

typedef struct _IMAGE_DOS_HEADER {      // DOS .EXE header
    UINT16   e_magic;                     // Magic number
    UINT16   e_cblp;                      // Bytes on last page of file
    UINT16   e_cp;                        // Pages in file
    UINT16   e_crlc;                      // Relocations
    UINT16   e_cparhdr;                   // Size of header in paragraphs
    UINT16   e_minalloc;                  // Minimum extra paragraphs needed
    UINT16   e_maxalloc;                  // Maximum extra paragraphs needed
    UINT16   e_ss;                        // Initial (relative) SS value
    UINT16   e_sp;                        // Initial SP value
    UINT16   e_csum;                      // Checksum
    UINT16   e_ip;                        // Initial IP value
    UINT16   e_cs;                        // Initial (relative) CS value
    UINT16   e_lfarlc;                    // File address of relocation table
    UINT16   e_ovno;                      // Overlay number
    UINT16   e_res[4];                    // Reserved words
    UINT16   e_oemid;                     // OEM identifier (for e_oeminfo)
    UINT16   e_oeminfo;                   // OEM information; e_oemid specific
    UINT16   e_res2[10];                  // Reserved words
    UINT32   e_lfanew;                    // File address of new exe header
} IMAGE_DOS_HEADER;

EFI_STATUS
LoadKernelImage (
    IN EFI_HANDLE   LoaderImageHandle,
    IN CHAR16       *FileName
    );

EFI_STATUS
efi_main (
    EFI_HANDLE          ImageHandle,
    EFI_SYSTEM_TABLE    *SystemTable
    )
{
    InitializeLib(ImageHandle, SystemTable);

    LoadKernelImage(ImageHandle, L"kernel.exe");

    WaitForKeyStroke(L"Press any key to exit...");

    return EFI_SUCCESS;
}

EFI_STATUS
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

    IMAGE_DOS_HEADER *DosHeader = KernelBuffer;
    IMAGE_NT_HEADERS64 *PeHeader =
        (IMAGE_NT_HEADERS64 *)((UINT64)DosHeader + DosHeader->e_lfanew);

    Print(L"Sections: %d\n", PeHeader->FileHeader.NumberOfSections);
    Print(L"Size of Optional Header: %d\n\n", PeHeader->FileHeader.SizeOfOptionalHeader);

    Print(L"Image Base:  %lx\n", PeHeader->OptionalHeader.ImageBase);
    Print(L"Entry Point: %lx\n\n", PeHeader->OptionalHeader.AddressOfEntryPoint);

    Print(L"File Alignment:    %lx\n", PeHeader->OptionalHeader.FileAlignment);
    Print(L"Section Alignment: %lx\n\n", PeHeader->OptionalHeader.SectionAlignment);

    return EFI_SUCCESS;
}
