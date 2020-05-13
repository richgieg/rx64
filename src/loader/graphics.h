#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <efi.h>
#include "../kernel/loader_info.h"

VOID
SetGraphicsMode (
    LOADER_INFO_GRAPHICS *LoaderInfoForGraphics
    );

#endif
