#include "util.h"
#include "graphics.h"

VOID
BugCheck ()
{
    // Red Screen of Destruction...
    FillScreen(0x00ff0000);

    for (;;) {
        asm("hlt");
    }
}
