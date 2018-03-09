#include <x86.h>
#include <intr.h>
#include <pic.h>
#include <fb.h>
#include <kbc.h>

void start_kernel(void *_t __attribute__ ((unused)), struct framebuffer *fb)
{
	fb_init(fb);
	set_fg(255, 255, 255);
	set_bg(0, 70, 250);
	clear_screen();

	gdt_init();
	intr_init();

	pic_init();
	set_intr_desc(INTR_NUM_KB, keyboard_handler);
	unsigned char mask = intr_get_mask_master();
	mask &= ~INTR_MASK_BIT_KB;
	intr_set_mask_master(mask);
	enable_cpu_intr();

	while (1)
		cpu_halt();
}
