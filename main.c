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
	/* フレームバッファ周りの初期化 */
	fb_init(_fb);
	set_fg(255, 255, 255);
	set_bg(0, 70, 250);
	clear_screen();

	fbcon_init();

	/* CPU周りの初期化 */
	gdt_init();
	intr_init();

	/* 周辺ICの初期化 */
	set_intr_desc(INTR_NUM_KB, keyboard_handler);
	unsigned char mask = intr_get_mask_master();
	mask &= ~INTR_MASK_BIT_KB;
	intr_set_mask_master(mask);

	/* CPUの割り込み有効化 */
	sti();

	/* haltして待つ */
	while (1)
		x86_halt();

	return 0;
}
