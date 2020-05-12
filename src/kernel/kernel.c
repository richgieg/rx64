void
KernelEntry(
    unsigned long FrameBufferBase,
    unsigned long FrameBufferSize
    )
{
    unsigned int    *GfxPtr;
    unsigned int    *GfxMax;
    unsigned int    Color;

    GfxMax = (unsigned int *)(FrameBufferBase + FrameBufferSize);
    Color = 0;

    for (;;) {
        GfxPtr = (unsigned int *)FrameBufferBase;
        while (GfxPtr != GfxMax) {
            *GfxPtr = Color;
            GfxPtr++;
        }
        Color++;
        if (Color == 0x1000000) {
            Color = 0;
        }
    }
}
