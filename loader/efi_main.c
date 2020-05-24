#include <efi.h>
#include <efilib.h>
#include <intrin.h>
#include "util.h"

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

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16

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

#define IMAGE_SIZEOF_SHORT_NAME              8

typedef struct _IMAGE_SECTION_HEADER {
    CHAR8   Name[IMAGE_SIZEOF_SHORT_NAME];
    union {
        UINT32 PhysicalAddress;
        UINT32 VirtualSize;
    } Misc;
    UINT32  VirtualAddress;
    UINT32  SizeOfRawData;
    UINT32  PointerToRawData;
    UINT32  PointerToRelocations;
    UINT32  PointerToLinenumbers;
    UINT16  NumberOfRelocations;
    UINT16  NumberOfLinenumbers;
    UINT32  Characteristics;
} IMAGE_SECTION_HEADER;

typedef struct _KERNEL_IMAGE_INFO {
    VOID (*kmain)();
} KERNEL_IMAGE_INFO;

EFI_STATUS
LoadKernelImage (
    IN EFI_HANDLE           LoaderImageHandle,
    IN CHAR16               *FileName,
    OUT KERNEL_IMAGE_INFO   **KernelImageInfo
    );

VOID
MapVirtualToPhysicalPages(
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress,
    IN UINTN    NumPages
    );

VOID
MapVirtualToPhysicalPage(
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress
    );

EFI_STATUS
efi_main (
    EFI_HANDLE          ImageHandle,
    EFI_SYSTEM_TABLE    *SystemTable
    )
{
    EFI_STATUS          Status;
    KERNEL_IMAGE_INFO   *KernelImageInfo;

    InitializeLib(ImageHandle, SystemTable);

    Status = LoadKernelImage(ImageHandle, L"kernel.exe", &KernelImageInfo);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to load kernel image\n");
        return Status;
    }

    Print(L"%lx\n", KernelImageInfo->kmain);

    WaitForKeyStroke(L"Press any key to enter kernel...");

    KernelImageInfo->kmain();

    return EFI_SUCCESS;
}

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

    IMAGE_DOS_HEADER *DosHeader;
    IMAGE_NT_HEADERS64 *NtHeader;
    IMAGE_SECTION_HEADER *SectionHeader;
    UINT8 *SectionData;
    EFI_PHYSICAL_ADDRESS PhysicalAddress;
    EFI_VIRTUAL_ADDRESS VirtualAddress;
    UINTN NumPages;

    DosHeader = (IMAGE_DOS_HEADER *)KernelBuffer;
    NtHeader = (IMAGE_NT_HEADERS64 *)((UINT64)DosHeader + DosHeader->e_lfanew);
    SectionHeader = (IMAGE_SECTION_HEADER *)(
        (UINT8 *)(&NtHeader->OptionalHeader) + NtHeader->FileHeader.SizeOfOptionalHeader);

    Print(L"Sections: %d\n", NtHeader->FileHeader.NumberOfSections);
    Print(L"Size of Optional Header: %d\n\n", NtHeader->FileHeader.SizeOfOptionalHeader);
    Print(L"Size of All Headers: %d\n\n", NtHeader->OptionalHeader.SizeOfHeaders);

    Print(L"Image Base:  %lx\n", NtHeader->OptionalHeader.ImageBase);
    Print(L"Entry Point: %lx\n\n", NtHeader->OptionalHeader.AddressOfEntryPoint);

    Print(L"File Alignment:    %lx\n", NtHeader->OptionalHeader.FileAlignment);
    Print(L"Section Alignment: %lx\n\n", NtHeader->OptionalHeader.SectionAlignment);

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
        Print(L"%x\n", *SectionData);
        Print(L"\n");

        SectionData += SectionHeader[i].SizeOfRawData;
    }

    *KernelImageInfo = AllocatePool(sizeof(KERNEL_IMAGE_INFO));
    (*KernelImageInfo)->kmain = (VOID *)(NtHeader->OptionalHeader.ImageBase +
        NtHeader->OptionalHeader.AddressOfEntryPoint);

    return EFI_SUCCESS;
}

VOID
MapVirtualToPhysicalPages(
    IN UINT64   VirtualAddress,
    IN UINT64   PhysicalAddress,
    IN UINTN    NumPages
    )
{
    UINTN i;

    for (i = 0; i < NumPages; i++) {
        MapVirtualToPhysicalPage(VirtualAddress, PhysicalAddress);
        VirtualAddress += EFI_PAGE_SIZE;
        PhysicalAddress += EFI_PAGE_SIZE;
    }
}

VOID
MapVirtualToPhysicalPage(
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

    // Address of PML4 is in CR3 register.
    Pml4 = (UINT64 *)__readcr3();
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
        ZeroMem(Pdpt, EFI_PAGE_SIZE);
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
        ZeroMem(Pd, EFI_PAGE_SIZE);
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
        ZeroMem(Pt, EFI_PAGE_SIZE);
        Pd[PdIndex] = (UINT64)Pt | 0x23;
        // Otherwise, get page table address from it.
    } else {
        Pt = (UINT64 *)(Pd[PdIndex] & 0x00fffffffffffff000);
    }

    // Write entry to page table.
    Pt[PtIndex] = PhysicalAddress | 0x23;
}
