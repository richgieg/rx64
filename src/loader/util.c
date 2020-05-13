#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include "util.h"

VOID
ExitBootServices (
    IN EFI_HANDLE           LoaderImageHandle
    )
{
    EFI_STATUS              Status;
    EFI_MEMORY_DESCRIPTOR   *MemoryMap;
    UINTN                   NoEntries;
    UINTN                   MapKey;
    UINTN                   DescriptorSize;
    UINT32                  DescriptorVersion;

    // Get most current memory map then exit boot services.
    MemoryMap = LibMemoryMap(&NoEntries, &MapKey, &DescriptorSize, &DescriptorVersion);
    Status = BS->ExitBootServices(LoaderImageHandle, MapKey);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to exit boot services\n");
        Exit(EFI_SUCCESS, 0, NULL);
    }
}

VOID *
LoadKernelImage (
    IN EFI_HANDLE   LoaderImageHandle,
    IN CHAR16       *FileName
    )
{
    EFI_STATUS              Status;
    EFI_LOADED_IMAGE        *LoadedImage;
    EFI_DEVICE_PATH         *DevicePath;
    EFI_FILE_IO_INTERFACE   *FileSystem;
    EFI_FILE_HANDLE         RootDirectory;
    EFI_FILE_HANDLE         KernelFileHandle;
    UINTN                   KernelBufferSize;
    VOID                    *KernelBuffer;
    Elf64_Ehdr              *ElfHeader;
    Elf64_Phdr              *ElfProgramHeader;
    EFI_PHYSICAL_ADDRESS    PhysicalAddress;
    UINTN                   NoPages;
    UINTN                   i;

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

    // Load kernel image into memory.
    Status = RootDirectory->Open(
        RootDirectory, &KernelFileHandle, FileName, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to open kernel image\n");
        Exit(EFI_SUCCESS, 0, NULL);
    }
    KernelBufferSize = 0x100000; // 1 MB
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

    Print(L"\nKernel Image Size:  %x\n\n", KernelBufferSize);

    // Parse kernel image and copy PT_LOAD segments to the required locations.
    ElfHeader = (Elf64_Ehdr *)KernelBuffer;
    for (i = 0; i < ElfHeader->e_phnum; i++) {
        ElfProgramHeader = ((Elf64_Phdr *)(KernelBuffer + ElfHeader->e_phoff)) + i;
        if (ElfProgramHeader->p_type != PT_LOAD) {
            continue;
        }
        NoPages = ElfProgramHeader->p_memsz / EFI_PAGE_SIZE + 1;
        PhysicalAddress = ElfProgramHeader->p_paddr;
        Status = BS->AllocatePages(AllocateAddress, EfiLoaderData, NoPages, &PhysicalAddress);
        if (EFI_ERROR(Status)) {
            Print(L"Failed to allocate physical memory at %x\n", i, PhysicalAddress);
            Exit(EFI_SUCCESS, 0, NULL);
        }
        // Zero all allocated memory in case segment contains BSS.
        // TODO: Parse ELF section headers and zero only if necessary.
        ZeroMem((VOID *)PhysicalAddress, NoPages * EFI_PAGE_SIZE);
        CopyMem(
            (VOID *)PhysicalAddress,
            (VOID *)(KernelBuffer + ElfProgramHeader->p_offset),
            ElfProgramHeader->p_filesz
        );
        Print(L"Physical Address:   %x\n", ElfProgramHeader->p_paddr);
        Print(L"Virtual Address:    %x\n", ElfProgramHeader->p_vaddr);
        Print(L"Image Offset:       %x\n", ElfProgramHeader->p_offset);
        Print(L"Size in Image:      %x\n", ElfProgramHeader->p_filesz);
        Print(L"Size in Memory:     %x\n", ElfProgramHeader->p_memsz);
        Print(L"Pages Allocated:    %x\n\n", NoPages);
    }

    FreePool(KernelBuffer);
    return (VOID *)ElfHeader->e_entry;
}

VOID
WaitForKeyStroke (
    IN CHAR16   *Prompt
    )
{
    if (Prompt != NULL) {
        Print(L"%s", Prompt);
    }
    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
    ST->ConIn->ReadKeyStroke(ST->ConIn, NULL);
}
