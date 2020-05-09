#include <efi.h>
#include <efilib.h>
#include "main.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    
    // TODO: Load kernel image.
    
    EFI_STATUS status;
    EFI_MEMORY_DESCRIPTOR *memoryMap;
    UINTN mapSize;
    UINTN descriptorSize;
    UINTN mapKey;
    status = GetMemoryMap(&memoryMap, &mapSize, &descriptorSize, &mapKey);
    if (status != EFI_SUCCESS) {
        return status;
    }

    // Print memory map for my own curiosity.
    PrintMemoryMap(memoryMap, mapSize, descriptorSize, TRUE);

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

EFI_STATUS GetMemoryMap(EFI_MEMORY_DESCRIPTOR **MemoryMap, UINTN *MapSize, UINTN *DescriptorSize, UINTN *MapKey) {
    EFI_STATUS status;
    EFI_MEMORY_DESCRIPTOR *memoryMap = NULL;
    UINTN mapSize = 0;
    UINTN descriptorSize;
    UINTN mapKey;
    UINT32 descriptorVersion;
    status = BS->GetMemoryMap(&mapSize, NULL, NULL, NULL, NULL);
    while (status == EFI_BUFFER_TOO_SMALL) {
        if (memoryMap) {
            status = BS->FreePool(memoryMap);
            if (status != EFI_SUCCESS) {
                break;
            }
        }
        status = BS->AllocatePool(EfiLoaderData, mapSize, (void **)&memoryMap);
        if (status != EFI_SUCCESS) {
            break;
        }
        status = BS->GetMemoryMap(&mapSize, memoryMap, &mapKey, &descriptorSize, &descriptorVersion);
    }
    if (status == EFI_SUCCESS) {
        *MemoryMap = memoryMap;
        *MapSize = mapSize;
        *DescriptorSize = descriptorSize;
        *MapKey = mapKey;
    }
    return status;
}

VOID PrintMemoryMap(EFI_MEMORY_DESCRIPTOR *MemoryMap, UINTN MapSize, UINTN DescriptorSize, BOOLEAN PrintDescriptors) {
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
    Print(L"Memory Map Size: %d\n", MapSize);
    Print(L"Descriptor Size: %d\n", DescriptorSize);
    if (PrintDescriptors) {
        UINT8 *startOfMemoryMap = (UINT8 *)MemoryMap;
        UINT8 *endOfMemoryMap = startOfMemoryMap + MapSize;
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
