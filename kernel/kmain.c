void kmain(unsigned __int32 *framebuffer) {

	for (int i = 0; i < 1920 * 1080; i++) {
		*framebuffer++ = 0xff;
	}

	for (;;);

}
