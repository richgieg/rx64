void
KernelEntry(
    unsigned long FrameBufferBase,
    unsigned long FrameBufferSize
    )
{
    unsigned int    *GfxPtr;
    unsigned int    *GfxMax;
    unsigned int    Color;

    GfxPtr = (unsigned int *)FrameBufferBase;
    GfxMax = (unsigned int *)(FrameBufferBase + FrameBufferSize);
    Color = 0x00ff0000;
    while (GfxPtr != GfxMax) {
        *GfxPtr = Color;
        GfxPtr++;
    }

    for (;;) {
        asm("hlt");
    }
}
