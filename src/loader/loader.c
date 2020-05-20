#include <efi.h>
#include <efilib.h>
#include <elf.h>
#include "graphics.h"
#include "recon.h"
#include "util.h"
#include "../kernel/loader_info.h"

VOID
MapFrameBuffer (
    EFI_PHYSICAL_ADDRESS FrameBufferBase
    );

EFI_STATUS
efi_main (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
    )
{
    LOADER_INFO                 *LoaderInfo;
    SET_GRAPHICS_MODE_RESULT    SetGraphicsModeResult;
    VOID                        (*KernelEntry)(LOADER_INFO *LoaderInfo);

    // Initialize EFI library (Set BS, RT, and ST globals).
    // BS = Boot Services, RT = Runtime Services, ST = System Table.
    InitializeLib(ImageHandle, SystemTable);
    // Disable watchdog timer (prevent system restart after 5 minutes).
    BS->SetWatchdogTimer(0, 0, 0, NULL);

    // // PrintEnvironmentVariables(FALSE);
    // // PrintMemoryMap(FALSE);
    KernelEntry = LoadKernelImage(ImageHandle, L"kernel.elf");
    // WaitForKeyStroke(L"\nPress any key to launch kernel...\n");
    LoaderInfo = AllocatePool(sizeof(LOADER_INFO));
    SetGraphicsMode(&SetGraphicsModeResult);
    LoaderInfo->FrameBufferBase = SetGraphicsModeResult.FrameBufferBase;
    LoaderInfo->FrameBufferSize = SetGraphicsModeResult.FrameBufferSize;
    LoaderInfo->HorizontalResolution = SetGraphicsModeResult.HorizontalResolution;
    LoaderInfo->VerticalResolution = SetGraphicsModeResult.VerticalResolution;
    ExitBootServices(ImageHandle);
    KernelEntry(LoaderInfo);

    // PrintMemoryMap(TRUE);
    // PrintControlRegisters();
    // PrintPml4Table(FALSE, FALSE);

    // MapFrameBuffer(LoaderInfo->Graphics.FrameBufferBase);

    // Kernel should never return, but if it does...
    for (;;) {
        asm("hlt");
    }
}

VOID
MapFrameBuffer (
    EFI_PHYSICAL_ADDRESS FrameBufferBase
    )
{
    UINT64 *Pml4Table;
    UINT64 *PdpTable;
    UINT64 *PageDirectory;
    UINT64 *PageTable;
    EFI_PHYSICAL_ADDRESS NewPages;
    UINT32 *VirtualFrameBuffer;
    int i;

    BS->AllocatePages(AllocateAnyPages, EfiLoaderData, 3, &NewPages);
    Pml4Table = (UINT64 *)GetPml4TableAddress();
    PdpTable = (UINT64 *)NewPages;
    PageDirectory = (UINT64 *)(NewPages + 4096);
    PageTable = (UINT64 *)(NewPages + 8192);

    PageTable[0] = FrameBufferBase & 0x0000fffffffff000 | 0x23;
    Print(L"%x\n", PageTable[0]);
    PageDirectory[0] = (UINT64)PageTable & 0x0000fffffffff000 | 0x23;
    Print(L"%x\n", PageDirectory[0]);
    PdpTable[0] = (UINT64)PageDirectory & 0x0000fffffffff000 | 0x23;
    Print(L"%x\n", PdpTable[0]);
    Pml4Table[256] = (UINT64)PdpTable & 0x0000fffffffff000 | 0x23;
    Print(L"%x\n", Pml4Table[256]);

    VirtualFrameBuffer = (UINT32 *)0xffff800000000000;
    for (i = 0; i < 1024; i++) {
        *VirtualFrameBuffer = 0xff0000;
        VirtualFrameBuffer++;
    }
}
