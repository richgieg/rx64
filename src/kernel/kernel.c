void
BltChar (
    unsigned long   FrameBufferBase,
    unsigned int    Column,
    unsigned int    Row,
    unsigned int    Color
    );

void
KernelEntry (
    unsigned long FrameBufferBase,
    unsigned long FrameBufferSize
    )
{
    unsigned int *GfxPtr;
    unsigned int *GfxMax;
    unsigned int Color;

    GfxPtr = (unsigned int *)FrameBufferBase;
    GfxMax = (unsigned int *)(FrameBufferBase + FrameBufferSize);
    Color = 0x00ff0000;
    while (GfxPtr != GfxMax) {
        *GfxPtr = Color;
        GfxPtr++;
    }

    BltChar(FrameBufferBase, 0, 0, 0x0000ff00);
    BltChar(FrameBufferBase, 239, 55, 0x000000ff);

    for (;;) {
        asm("hlt");
    }
}

void
BltChar (
    unsigned long   FrameBufferBase,
    unsigned int    Column,
    unsigned int    Row,
    unsigned int    Color
    )
{
    unsigned int *GfxPtr;
    unsigned int PixelsPerScanLine;
    unsigned int Width;
    unsigned int Height;
    unsigned int i;
    unsigned int j;

    PixelsPerScanLine = 1920;
    Width = 8;
    Height = 19;

    for (i = 0; i < Height; i++) {
        GfxPtr = ((unsigned int *)FrameBufferBase) + (i * PixelsPerScanLine) + (Row * Height * PixelsPerScanLine) + (Column * Width);
        for (j = 0; j < Width; j++) {
            *GfxPtr = Color;
            GfxPtr++;
        }
    }

}
