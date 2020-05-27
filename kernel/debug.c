#include <debug.h>
#include <console.h>
#include <console_.h>
#include <intrin.h>

VOID
DbgHalt (
    IN CHAR16 *Message
    )
{
    if (CnConsoleInitialized()) {
        CnSetBackgroundColor(0xff0000);
        CnSetForegroundColor(0xffffff);
        CnClear();
        CnPrint(L"Rx64 detected a fatal error and has halted.\n\n");
        if (Message != NULL) {
            CnPrint(Message);
            CnPrint(L"\n\n");
        }
        CnPrint(L"You may turn off your computer.");
    }

    for (;;) {
        __halt();
    }
}
