#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "types.h"

VOID
InitializeConsole ();

VOID
Print(
    CONST CHAR16 *String
    );

VOID
PutChar (
    UINT16 Column,
    UINT16 Row,
    CHAR16 Char
    );

#endif
