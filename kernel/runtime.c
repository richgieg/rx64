#include <runtime.h>
#include <debug.h>

VOID
RtCopyMemory (
    IN VOID         *Destination,
    IN CONST VOID   *Source,
    IN UINT64       Length
    )
{
    // TODO: Remove 8-byte boundary limitation!
    if ((UINT64)Destination % 8 != 0) {
        DbgHalt(L"RtCopyMemory argument 'Destination' is not on an 8-byte boundary.");
    }
    if ((UINT64)Source % 8 != 0) {
        DbgHalt(L"RtCopyMemory argument 'Source' is not on an 8-byte boundary.");
    }
    if (Length % 8 != 0) {
        DbgHalt(L"RtCopyMemory argument 'Length' is not on an 8-byte boundary.");
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
