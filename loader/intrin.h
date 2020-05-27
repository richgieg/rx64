#pragma once
// intrin.h
//
// Intrinsic functions in Microsoft C++ compiler that are used by the loader.
//
// List of all intrinsic functions:
// https://docs.microsoft.com/en-us/cpp/intrinsics/x64-amd64-intrinsics-list
//

#pragma once

#include <types.h>

VOID    __halt();
UINT64  __readcr3();
