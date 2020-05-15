#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "types.h"

VOID
CnInitializeConsole ();

VOID
CnPrint(
    CONST CHAR16 *String
    );

VOID
CnPutChar (
    UINT16 Column,
    UINT16 Row,
    CHAR16 Char
    );

#endif
