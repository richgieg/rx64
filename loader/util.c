#include <efi.h>
#include <efilib.h>
#include "util.h"

VOID
WaitForKeyStroke(
    IN CHAR16 *Prompt
)
{
    if (Prompt != NULL) {
        Print(L"%s", Prompt);
    }
    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
    ST->ConIn->ReadKeyStroke(ST->ConIn, NULL);
}
