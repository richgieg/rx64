#include <efi.h>
#include <efilib.h>
#include "main.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    
    // TODO: Load kernel image.
    
    EFI_MEMORY_DESCRIPTOR *memoryMap;
    UINTN noEntries;
    UINTN mapKey;
    UINTN descriptorSize;
    UINT32 descriptorVersion;
    memoryMap = LibMemoryMap(&noEntries, &mapKey, &descriptorSize, &descriptorVersion);

    // Print memory map for my own curiosity.
    PrintMemoryMap(memoryMap, noEntries, descriptorSize, TRUE);

    // TODO: Exit boot services and transfer control to kernel.

    // status = BS->ExitBootServices(ImageHandle, mapKey);
    // if (status != EFI_SUCCESS) {
    //     return status;
    // }

    Print(L"\n\nPress any key to shut down...\n");
    WaitForKeyStroke();
    RT->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
    
    // Ensure this function does not return.
    for (;;) asm("hlt");
}

VOID PrintMemoryMap(EFI_MEMORY_DESCRIPTOR *MemoryMap, UINTN NoEntries, UINTN DescriptorSize, BOOLEAN PrintDescriptors) {
    const CHAR16 *memoryTypes[] = {
        L"EfiReservedMemoryType",
        L"EfiLoaderCode",
        L"EfiLoaderData",
        L"EfiBootServicesCode",
        L"EfiBootServicesData",
        L"EfiRuntimeServicesCode",
        L"EfiRuntimeServicesCode",
        L"EfiConventionalMemory",
        L"EfiUnusableMemory",
        L"EfiACPIReclaimMemory",
        L"EfiACPIReclaimMemory",
        L"EfiMemoryMappedIO",
        L"EfiMemoryMappedIOPortSpace",
        L"EfiPalCode",
        L"EfiMaxMemoryType"
    };
    UINTN mapSize = NoEntries * DescriptorSize;
    Print(L"Memory Map Size: %d\n", mapSize);
    Print(L"Descriptor Size: %d\n", DescriptorSize);
    if (PrintDescriptors) {
        UINT8 *startOfMemoryMap = (UINT8 *)MemoryMap;
        UINT8 *endOfMemoryMap = startOfMemoryMap + mapSize;
        UINT8 *offset = startOfMemoryMap;
        UINT32 counter = 0;
        while (offset < endOfMemoryMap)
        {
            EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)offset;
            Print(L"\nDescriptor %d:\n", counter);
            Print(L"  Type: %x, %s\n", desc->Type, memoryTypes[desc->Type]); 
            Print(L"  PhysicalStart: %x\n", desc->PhysicalStart);
            Print(L"  VirtualStart: %x\n", desc->VirtualStart);
            Print(L"  NumberOfPages: %x\n", desc->NumberOfPages);
            Print(L"  Attribute: %x\n", desc->Attribute);
            offset += DescriptorSize;
            counter++;
            WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
            ST->ConIn->ReadKeyStroke(ST->ConIn, NULL);
        }
    }
}

VOID WaitForKeyStroke() {
    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
    ST->ConIn->ReadKeyStroke(ST->ConIn, NULL);
}
