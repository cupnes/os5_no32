#include <x86.h>
#include <excp.h>
#include <intr.h>
#include <efi.h>
#include <fb.h>
#include <fbcon.h>
#include <kbc.h>

int kern_init(struct EFI_SYSTEM_TABLE *st __attribute__ ((unused)),
	      struct fb *_fb)
{
	fb_init(_fb);
	set_fg(255, 255, 255);
	set_bg(0, 70, 250);
	clear_screen();

	fbcon_init();

	gdt_init();
	intr_init();

	set_intr_desc(INTR_NUM_KB, keyboard_handler);
	unsigned char mask = intr_get_mask_master();
	mask &= ~INTR_MASK_BIT_KB;
	intr_set_mask_master(mask);
	sti();

	while (1)
		x86_halt();

	return 0;
}
