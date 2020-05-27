#pragma once

#include <efi.h>
#include "../kernel/loader_info.h"

EFI_STATUS
GetGraphicsInfo (
    OUT LOADER_GRAPHICS_INFO **GraphicsInfo
    );
