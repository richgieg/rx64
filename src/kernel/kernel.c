void KernelEntry() {
    for (;;) {
        asm("hlt");
    }
}
