#ifndef LOADER_INFO_H_
#define LOADER_INFO_H_

typedef struct _LOADER_INFO_GRAPHICS {
    unsigned long               FrameBufferBase;
    unsigned long               FrameBufferSize;
} LOADER_INFO_GRAPHICS;

typedef struct _LOADER_INFO {
    LOADER_INFO_GRAPHICS        Graphics;
} LOADER_INFO;

#endif
