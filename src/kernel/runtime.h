#ifndef RUNTIME_H_
#define RUNTIME_H_

#include "types.h"

VOID
RtCopyMemory (
    IN VOID         *Destination,
    IN CONST VOID   *Source,
    IN UINTN        Length
    );

#endif
