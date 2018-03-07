#include <x86.h>
#include <excp.h>
#include <intr.h>
#include <fb.h>
#include <fbcon.h>
#include <kbc.h>

void start_kernel(void *_t __attribute__ ((unused)), struct framebuffer *fb)
{
	/* フレームバッファ周りの初期化 */
	fb_init(fb);
	set_fg(255, 255, 255);
	set_bg(0, 70, 250);
	clear_screen();

	/* CPU周りの初期化 */
	gdt_init();
	intr_init();

	/* 周辺ICの初期化 */
	set_intr_desc(INTR_NUM_KB, keyboard_handler);
	unsigned char mask = intr_get_mask_master();
	mask &= ~INTR_MASK_BIT_KB;
	intr_set_mask_master(mask);

	/* CPUの割り込み有効化 */
	enable_cpu_intr();

	/* haltして待つ */
	while (1)
		cpu_halt();
}
