#pragma once

typedef     unsigned __int64    UINT64;
typedef     unsigned __int32    UINT32;
typedef     unsigned __int16    UINT16;
typedef     unsigned __int8     UINT8;

typedef     __int64             INT64;
typedef     __int32             INT32;
typedef     __int16             INT16;
typedef     __int8              INT8;

typedef     UINT16              CHAR16;
typedef     UINT8               CHAR8;
typedef     UINT8               BYTE;
typedef     UINT8               BOOLEAN;

#define VOID void

#ifndef CONST
    #define CONST const
#endif
#ifndef TRUE
    #define TRUE    ((BOOLEAN) 1)
    #define FALSE   ((BOOLEAN) 0)
#endif

#ifndef NULL
    #define NULL    ((VOID *) 0)
#endif

#ifndef IN
    #define IN          // Argument passed into function
    #define OUT         // Argument (pointer) returned from function
    #define OPTIONAL    // Argument is optional
#endif
