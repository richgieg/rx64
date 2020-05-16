#include "kernel.h"
#include "runtime.h"

VOID
RtCopyMemory (
    IN VOID         *Destination,
    IN CONST VOID   *Source,
    IN UINTN        Length
    )
{
    // TODO: Optimize by transferring 8 bytes at a time.
    while (Length--) {
        *(UINT8 *)Destination++ = *(UINT8 *)Source++;
    }
}
