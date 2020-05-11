char *test = "asdf";

void KernelEntry() {
    test[0] = 'z';
    for (;;) {
        asm("hlt");
    }
}
