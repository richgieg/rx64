#pragma once

#include <efi.h>
#include "../kernel/kmain.h"

EFI_STATUS
GetGraphicsInfo (
    LOADER_GRAPHICS_INFO **GraphicsInfo
    );
