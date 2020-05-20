#include <efi.h>
#include <efilib.h>
#include "graphics.h"

VOID
SetGraphicsMode (
    IN SET_GRAPHICS_MODE_RESULT *Result
    )
{
    EFI_STATUS Status;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput;
    UINT32 Mode;
    UINTN SizeOfInfo;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;

    Status = BS->LocateProtocol(&GraphicsOutputProtocol, NULL, (VOID **)&GraphicsOutput);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to get graphics output handle\n");
        Exit(EFI_SUCCESS, 0, NULL);
    }
    for (Mode = 0; Mode < GraphicsOutput->Mode->MaxMode; Mode++) {
        Status = GraphicsOutput->QueryMode(GraphicsOutput, Mode, &SizeOfInfo, &Info);
        if (EFI_ERROR(Status)) {
            Print(L"Failed to query graphics mode %d\n", Mode);
            Exit(EFI_SUCCESS, 0, NULL);
        }
        if (Info->HorizontalResolution == 1920 &&
            Info->VerticalResolution == 1080 &&
            Info->PixelFormat == PixelBlueGreenRedReserved8BitPerColor) {
            break;
        }
        if (Mode == GraphicsOutput->Mode->MaxMode) {
            Print(L"Required graphics mode not supported\n");
            Exit(EFI_SUCCESS, 0, NULL);
        }
    }
    Status = GraphicsOutput->SetMode(GraphicsOutput, Mode);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to set graphics mode\n");
        Exit(EFI_SUCCESS, 0, NULL);
    }

    Result->FrameBufferBase = GraphicsOutput->Mode->FrameBufferBase;
    Result->FrameBufferSize = GraphicsOutput->Mode->FrameBufferSize;
    Result->HorizontalResolution = GraphicsOutput->Mode->Info->HorizontalResolution;
    Result->VerticalResolution = GraphicsOutput->Mode->Info->VerticalResolution;
}
