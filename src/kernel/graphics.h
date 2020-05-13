#ifndef GRAPHICS_H_
#define GRAPHICS_H_

void
InitializeGraphics (
    unsigned long FrameBufferBase,
    unsigned long FrameBufferSize
    );

void
FillScreen (
    unsigned int Color
    );

void
FillChar (
    unsigned int    Column,
    unsigned int    Row,
    unsigned int    Color
    );

#endif
