#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include "util.h"

VOID
ExitBootServices (
    IN EFI_HANDLE               LoaderImageHandle,
    IN CONTIGUOUS_MEMORY_INFO   *ContiguousPhysicalMemoryInfo
    )
{
    EFI_STATUS              Status;
    EFI_MEMORY_DESCRIPTOR   *MemoryMap;
    EFI_MEMORY_DESCRIPTOR   *MemoryMapEntry;
    UINTN                   NoEntries;
    UINTN                   MapKey;
    UINTN                   DescriptorSize;
    UINT32                  DescriptorVersion;
    UINTN                   NoPages;
    UINTN                   i;

    // Get most current memory map then exit boot services.
    MemoryMap = LibMemoryMap(&NoEntries, &MapKey, &DescriptorSize, &DescriptorVersion);
    Status = BS->ExitBootServices(LoaderImageHandle, MapKey);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to exit boot services\n");
        Exit(EFI_SUCCESS, 0, NULL);
    }

    // Get largest block of contiguous physical memory.
    MemoryMapEntry = MemoryMap;
    ContiguousPhysicalMemoryInfo->NoPages = 0;
    for (i = 0; i < NoEntries; i++) {
        if (MemoryMapEntry->Type == EfiConventionalMemory &&
            MemoryMapEntry->NumberOfPages > ContiguousPhysicalMemoryInfo->NoPages) {
            ContiguousPhysicalMemoryInfo->BaseAddress = MemoryMapEntry->PhysicalStart;
            ContiguousPhysicalMemoryInfo->NoPages = MemoryMapEntry->NumberOfPages;
        }
        MemoryMapEntry = NextMemoryDescriptor(MemoryMapEntry, DescriptorSize);
    }
}

EFI_PHYSICAL_ADDRESS
GetPml4TableAddress ()
{
    EFI_PHYSICAL_ADDRESS    Pml4TableAddress;
    UINT64                  CR3;

    asm(
        "mov %%cr3, %%rax;"
        "mov %%rax, %0;"
        :"=r"(CR3)
        :
        :"%rax"
    );
    // Ignore bits 0 through 11 (PWT and PCD bits, etc.).
    Pml4TableAddress = CR3 & 0xfffffffffffff000;
    return Pml4TableAddress;
}

KERNEL_IMAGE_INFO *
LoadKernelImage (
    IN EFI_HANDLE           LoaderImageHandle,
    IN CHAR16               *FileName
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
    Elf64_Ehdr                  *ElfHeader;
    Elf64_Phdr                  *ElfProgramHeader;
    EFI_PHYSICAL_ADDRESS        PhysicalAddress;
    EFI_VIRTUAL_ADDRESS         VirtualAddress;
    KERNEL_IMAGE_INFO           *KernelImageInfo;
    LOADER_PAGE_MAPPING_INFO    *KernelSection;
    UINTN                       NoPages;
    UINTN                       i;

    // Allocate enough to hold KERNEL_IMAGE_INFO (has variable
    // array which holds info about each section in the image).
    KernelImageInfo = AllocatePool(EFI_PAGE_SIZE);

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

    // Parse kernel image and copy PT_LOAD segments to the required locations.
    ElfHeader = (Elf64_Ehdr *)KernelBuffer;
    KernelImageInfo->KernelEntry = (VOID *)ElfHeader->e_entry;
    KernelImageInfo->NoSectionMappings = 0;
    KernelSection = KernelImageInfo->SectionMappings;

    for (i = 0; i < ElfHeader->e_phnum; i++) {
        ElfProgramHeader = ((Elf64_Phdr *)(KernelBuffer + ElfHeader->e_phoff)) + i;
        if (ElfProgramHeader->p_type != PT_LOAD) {
            continue;
        }
        NoPages = ElfProgramHeader->p_memsz / EFI_PAGE_SIZE;
        if (ElfProgramHeader->p_memsz % EFI_PAGE_SIZE) {
            NoPages++;
        };
        Status = BS->AllocatePages(AllocateAnyPages, EfiLoaderData, NoPages, &PhysicalAddress);
        if (EFI_ERROR(Status)) {
            Print(L"Failed to allocate pages for kernel section %d\n", i);
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
        MapVirtualToPhysicalPages(ElfProgramHeader->p_vaddr, PhysicalAddress, NoPages);

        KernelSection->NoPages = NoPages;
        KernelSection->PhysicalAddress = PhysicalAddress;
        KernelSection->VirtualAddress = ElfProgramHeader->p_vaddr;
        KernelSection++;
        KernelImageInfo->NoSectionMappings++;
    }

    FreePool(FileInfo);
    FreePool(KernelBuffer);

    return KernelImageInfo;
}

VOID
MapVirtualToPhysicalPages (
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress,
    IN UINTN    NoPages
    )
{
    UINTN i;

    for (i = 0; i < NoPages; i++) {
        MapVirtualToPhysicalPage(VirtualAddress, PhysicalAddress);
        VirtualAddress += i * EFI_PAGE_SIZE;
        PhysicalAddress += i * EFI_PAGE_SIZE;
    }
}

VOID
MapVirtualToPhysicalPage (
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress
    )
{
    EFI_STATUS  Status;
    UINT64      *Pml4;
    UINT64      *Pdpt;
    UINT64      *Pd;
    UINT64      *Pt;
    UINTN       Pml4Index;
    UINTN       PdptIndex;
    UINTN       PdIndex;
    UINTN       PtIndex;

    Pml4 = (UINT64 *)GetPml4TableAddress();
    Pml4Index = (VirtualAddress >> 39) & 0x1ff;
    PdptIndex = (VirtualAddress >> 30) & 0x1ff;
    PdIndex = (VirtualAddress >> 21) & 0x1ff;
    PtIndex = (VirtualAddress >> 12) & 0x1ff;

    // If PML4 entry not present then allocate a page for its
    // corresponding page directory pointer table.
    if (!(Pml4[Pml4Index] & 1)) {
        Status = BS->AllocatePages(AllocateAnyPages, EfiLoaderData, 1, (EFI_PHYSICAL_ADDRESS *)&Pdpt);
        if (EFI_ERROR(Status)) {
            Print(L"Failed to allocate page for PDPT\n");
            Exit(EFI_SUCCESS, 0, NULL);
        }
        Pml4[Pml4Index] = (UINT64)Pdpt | 0x23;
    // Otherwise, get page directory pointer table address from it.
    } else {
        Pdpt = (UINT64 *)(Pml4[Pml4Index] & 0x00fffffffffffff000);
    }

    // If page directory pointer table entry not present then
    // allocate a page for its corresponding page directory.
    if (!(Pdpt[PdptIndex] & 1)) {
        Status = BS->AllocatePages(AllocateAnyPages, EfiLoaderData, 1, (EFI_PHYSICAL_ADDRESS *)&Pd);
        if (EFI_ERROR(Status)) {
            Print(L"Failed to allocate page for PD\n");
            Exit(EFI_SUCCESS, 0, NULL);
        }
        Pdpt[PdptIndex] = (UINT64)Pd | 0x23;
    // Otherwise, get page directory address from it.
    } else {
        Pd = (UINT64 *)(Pdpt[PdptIndex] & 0x00fffffffffffff000);
    }

    // If page directory entry not present then allocate a page
    // for its corresponding page table.
    if (!(Pd[PdIndex] & 1)) {
        Status = BS->AllocatePages(AllocateAnyPages, EfiLoaderData, 1, (EFI_PHYSICAL_ADDRESS *)&Pt);
        if (EFI_ERROR(Status)) {
            Print(L"Failed to allocate page for PT\n");
            Exit(EFI_SUCCESS, 0, NULL);
        }
        Pd[PdIndex] = (UINT64)Pt | 0x23;
    // Otherwise, get page table address from it.
    } else {
        Pt = (UINT64 *)(Pd[PdIndex] & 0x00fffffffffffff000);
    }

    // Write entry to page table.
    Pt[PtIndex] = PhysicalAddress | 0x23;
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
