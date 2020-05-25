#pragma once

#include <types.h>

VOID
CnInitializeConsole ();

VOID
CnPrint(
    IN CONST CHAR16 *String
    );

VOID
CnPutChar (
    IN UINT16 Column,
    IN UINT16 Row,
    IN CHAR16 Char
    );
