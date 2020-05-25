#include <runtime.h>

VOID
RtCopyMemory (
    IN VOID         *Destination,
    IN CONST VOID   *Source,
    IN UINT64       Length
    )
{
    // TODO: Make this function not stupid...
    if ((UINT64)Destination % 8 != 0) {
        //KeBugCheck();
    }
    if ((UINT64)Source % 8 != 0) {
        //KeBugCheck();
    }
    if (Length % 8 != 0) {
        //KeBugCheck();
    }

    UINT64 *Dst;
    UINT64 *Src;
    UINT64 Len;

    Dst = (UINT64 *)Destination;
    Src = (UINT64 *)Source;
    Len = Length / 8;

    while (Len--) {
        *Dst++ = *Src++;
    }
}
