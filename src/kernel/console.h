#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "types.h"

VOID
InitializeConsole ();

VOID
PutChar (
    UINT16 Column,
    UINT16 Row,
    CHAR16 Char
    );

#endif
