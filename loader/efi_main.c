#include <efi.h>
#include <efilib.h>

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	UINTN Event;

	InitializeLib(ImageHandle, SystemTable);

	Print(L"Press any key to exit...\n");

	ST->ConIn->Reset(ST->ConIn, FALSE);
	ST->BootServices->WaitForEvent(1, &ST->ConIn->WaitForKey, &Event);

	return EFI_SUCCESS;
}
