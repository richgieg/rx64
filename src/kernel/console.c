#include "console.h"
#include "graphics.h"

static UINT16 Column;
static UINT16 Row;

VOID
InitializeConsole ()
{
    FillScreen(0);
}
