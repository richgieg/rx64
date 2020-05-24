#pragma once

#include <efi.h>

VOID
WaitForKeyStroke(
    IN CHAR16 *Prompt
);
