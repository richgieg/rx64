#include <efi.h>
#include <efilib.h>
#include "graphics.h"
#include "memory.h"

#define FRAME_BUFFER_VIRTUAL_BASE 0xffffff8000000000

EFI_STATUS
GetGraphicsInfo (
    GRAPHICS_INFO **GraphicsInfo
    )
{
    EFI_STATUS                      Status;
    EFI_GRAPHICS_OUTPUT_PROTOCOL    *GraphicsOutput;
    UINTN                           NumPages;

    Status = BS->LocateProtocol(&GraphicsOutputProtocol, NULL, &GraphicsOutput);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to get graphics output handle\n");
        return Status;
    }

    if (GraphicsOutput->Mode->Info->PixelFormat != PixelBlueGreenRedReserved8BitPerColor) {
        Print(L"Current pixel format not supported\n");
        return EFI_UNSUPPORTED;
    }

    NumPages = CalculatePagesFromBytes(GraphicsOutput->Mode->FrameBufferSize);
    MapVirtualToPhysicalPages(FRAME_BUFFER_VIRTUAL_BASE,
        GraphicsOutput->Mode->FrameBufferBase, NumPages);

    *GraphicsInfo = AllocatePool(sizeof(GRAPHICS_INFO));
    (*GraphicsInfo)->FrameBufferBase = FRAME_BUFFER_VIRTUAL_BASE;
    (*GraphicsInfo)->HorizontalResolution = GraphicsOutput->Mode->Info->HorizontalResolution;
    (*GraphicsInfo)->VerticalResolution = GraphicsOutput->Mode->Info->VerticalResolution;

    return EFI_SUCCESS;
}
