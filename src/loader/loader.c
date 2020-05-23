#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include "graphics.h"
#include "recon.h"
#include "util.h"
#include "../kernel/loader_info.h"

EFI_STATUS
efi_main (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
    )
{
    LOADER_INFO                 *LoaderInfo;
    KERNEL_IMAGE_INFO           *KernelImageInfo;
    SET_GRAPHICS_MODE_RESULT    SetGraphicsModeResult;
    CONTIGUOUS_MEMORY_INFO      ContiguousPhysicalMemoryInfo;

    // Initialize EFI library (Set BS, RT, and ST globals).
    // BS = Boot Services, RT = Runtime Services, ST = System Table.
    InitializeLib(ImageHandle, SystemTable);
    // Disable watchdog timer (prevent system restart after 5 minutes).
    BS->SetWatchdogTimer(0, 0, 0, NULL);

    // PrintEnvironmentVariables(FALSE);
    // PrintMemoryMap(FALSE);

    LoaderInfo = AllocatePool(sizeof(LOADER_INFO));
    KernelImageInfo = LoadKernelImage(ImageHandle, L"kernel.elf");
    SetGraphicsMode(&SetGraphicsModeResult);
    LoaderInfo->NoKernelSectionMappings = KernelImageInfo->NoSectionMappings;
    LoaderInfo->KernelSectionMappings = KernelImageInfo->SectionMappings;
    LoaderInfo->FrameBufferBase = SetGraphicsModeResult.FrameBufferBase;
    LoaderInfo->FrameBufferSize = SetGraphicsModeResult.FrameBufferSize;
    LoaderInfo->HorizontalResolution = SetGraphicsModeResult.HorizontalResolution;
    LoaderInfo->VerticalResolution = SetGraphicsModeResult.VerticalResolution;
    ExitBootServices(ImageHandle, &ContiguousPhysicalMemoryInfo);
    LoaderInfo->ContiguousPhysicalMemoryAddress = ContiguousPhysicalMemoryInfo.BaseAddress;
    LoaderInfo->NoContinguousPhysicalMemoryPages = ContiguousPhysicalMemoryInfo.NoPages;
    KernelImageInfo->KernelEntry(LoaderInfo);

    // Kernel should never return, but if it does...
    for (;;) {
        asm("hlt");
    }
}
