#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include "loader.h"

static CHAR16 *KernelFileName = L"kernel.elf";

EFI_STATUS
efi_main (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
    )
{
    EFI_STATUS              Status;
    EFI_TIME                Time;

    EFI_MEMORY_DESCRIPTOR   *MemoryMap;
    EFI_MEMORY_DESCRIPTOR   *MemoryMapEntry;
    UINTN                   NoEntries;
    UINTN                   MapKey;
    UINTN                   DescriptorSize;
    UINT32                  DescriptorVersion;

    VOID                    (*KernelEntry)();

    // Initialize the library (Set BS, RT, and ST globals).
    // BS = Boot Services, RT = Runtime Services, ST = System Table.
    InitializeLib(ImageHandle, SystemTable);

    // Print message with date and time.
    Print(L"Rx64 loader started\n");
    Status = RT->GetTime(&Time, NULL);
    if (!EFI_ERROR(Status)) {
        Print(L"%02d/%02d/%04d %02d:%02d:%02d\n",
            Time.Month, Time.Day, Time.Year, Time.Hour, Time.Minute, Time.Second);
    }

    WaitForKeyStroke(L"\nPress any key to continue...\n");

    // Display current memory map.
    MemoryMap = LibMemoryMap(&NoEntries, &MapKey, &DescriptorSize, &DescriptorVersion);
    if (MemoryMap == NULL) {
        Print(L"Failed to retrieve memory map\n");
        Exit(EFI_SUCCESS, 0, NULL);
    }
    PrintMemoryMap(MemoryMap, NoEntries, DescriptorSize, FALSE);
    FreePool(MemoryMap);

    WaitForKeyStroke(L"\nPress any key to continue...\n");

    // Display environment variables.
    PrintEnvironmentVariables(FALSE);

    WaitForKeyStroke(L"\nPress any key to load kernel image...\n");

    KernelEntry = LoadKernelImage(ImageHandle);

    WaitForKeyStroke(L"Press any key to enter kernel...\n");

    // Get most current memory map and exit boot services.
    MemoryMap = LibMemoryMap(&NoEntries, &MapKey, &DescriptorSize, &DescriptorVersion);
    Status = BS->ExitBootServices(ImageHandle, MapKey);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to exit boot services\n");
        Exit(EFI_SUCCESS, 0, NULL);
    }

    KernelEntry();

    // Kernel should never return, but if it does...
    for (;;) {
        asm("hlt");
    }
}

VOID *
LoadKernelImage (
    IN EFI_HANDLE   LoaderImageHandle
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
        LoadedImage->DeviceHandle, &FileSystemProtocol,(VOID **)&FileSystem);
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
        RootDirectory, &KernelFileHandle, KernelFileName, EFI_FILE_MODE_READ, 0);
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
PrintMemoryMap (
    IN EFI_MEMORY_DESCRIPTOR    *MemoryMap,
    IN UINTN                    NoEntries,
    IN UINTN                    DescriptorSize,
    IN BOOLEAN                  PauseAfterEntry
    )
{
    const CHAR16 *MemoryTypes[EfiMaxMemoryType] = {
        L"Reserved               ",
        L"Loader Code            ",
        L"Loader Data            ",
        L"Boot Services Code     ",
        L"Boot Services Data     ",
        L"Runtime Services Code  ",
        L"Runtime Services Data  ",
        L"Conventional           ",
        L"Unusable               ",
        L"ACPI Reclaim           ",
        L"ACPI NVS               ",
        L"Memory-Mapped I/O      ",
        L"Memory-Mapped I/O Ports",
        L"Pal Code               "
    };

    EFI_MEMORY_DESCRIPTOR   *MemoryMapEntry;
    UINTN                   i;

    Print(L"\nMemory Descriptors (press key to cycle through):\n\n");
    Print(L"Type                     Start Address     End Address       Attributes      \n");
    Print(L"=======================  ================  ================  ================\n");
    MemoryMapEntry = MemoryMap;
    for (i = 0; i < NoEntries; i++) {
        Print(L"%s  %lX  %lX  %lX\n",
            MemoryTypes[MemoryMapEntry->Type],
            MemoryMapEntry->PhysicalStart,
            MemoryMapEntry->PhysicalStart +
                LShiftU64(MemoryMapEntry->NumberOfPages, EFI_PAGE_SHIFT) - 1,
            MemoryMapEntry->Attribute);
        MemoryMapEntry = NextMemoryDescriptor(MemoryMapEntry, DescriptorSize);
        if (PauseAfterEntry) {
            WaitForKeyStroke(NULL);
        }
    }
}

VOID
PrintEnvironmentVariables (
    IN BOOLEAN  PauseAfterEntry
    )
{
    EFI_STATUS  Status;
    UINTN       VariableNameSize;
    CHAR16      VariableName[256];
    EFI_GUID    VendorGuid;
    UINT8       *VariableValue;

    Print(L"\nEnvironment Variables (press key to cycle through):\n\n");
    VariableName[0] = 0x0000;
    VendorGuid = NullGuid;
    Print(L"GUID                                Variable Name        Value Address   \n");
    Print(L"=================================== ==================== ================\n");
    do {
        VariableNameSize = sizeof(VariableName);
        Status = RT->GetNextVariableName(&VariableNameSize, VariableName, &VendorGuid);
        if (Status == EFI_SUCCESS) {
            VariableValue = LibGetVariable(VariableName, &VendorGuid);
            if (VariableValue != NULL) {
                Print(L"%.-35g %.-20s %lX\n", &VendorGuid, VariableName, VariableValue);
                FreePool(VariableValue);
                if (PauseAfterEntry) {
                    WaitForKeyStroke(NULL);
                }
            }
        }
    } while (Status == EFI_SUCCESS);
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
