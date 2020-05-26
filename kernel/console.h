#pragma once

#include <types.h>

VOID
CnPrint (
    IN CONST CHAR16 *String
    );

VOID
CnPrintHex (
    IN UINT64 Value
    );

VOID
CnPrintHexWithPad (
    IN UINT64   Value,
    IN UINT64   MinimumWidth
    );

VOID
CnPutChar (
    IN UINT16 Column,
    IN UINT16 Row,
    IN CHAR16 Char
    );

VOID
CnClear ();

VOID
CnSetBackgroundColor (
    IN UINT32 Color
    );

VOID
CnSetForegroundColor (
    IN UINT32 Color
    );

VOID
Cn__Demo__PrintColorfulChars ();

VOID
Cn__Demo__PrintMemory ();
