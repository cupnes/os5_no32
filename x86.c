#include <x86.h>

unsigned long long gdtr[2];

const unsigned long long gdt[] = {
	0x0000000000000000,	/* NULL descriptor */
	0x00af9a000000ffff,	/* base=0, limit=4GB, mode=code(r-x),kernel */
	0x00cf93000000ffff	/* base=0, limit=4GB, mode=data(rw-),kernel */
};

void gdt_init(void)
{
	/* GDTRの設定 */
	gdtr[0] = ((unsigned long long)gdt << 16) | (sizeof(gdt) - 1);
	gdtr[1] = ((unsigned long long)gdt >> 48);
	asm volatile ("lgdt gdtr");

	/* DS・SSの設定 */
	asm volatile ("movw $2*8, %ax\n"
		      "movw %ax, %ds\n"
		      "movw %ax, %ss\n");

	/* CSの設定 */
	unsigned short selector = SS_KERNEL_CODE;
	unsigned long long dummy;
	asm volatile ("pushq %[selector];"
		      "leaq ret_label(%%rip), %[dummy];"
		      "pushq %[dummy];"
		      "lretq;"
		      "ret_label:"
		      : [dummy]"=r"(dummy)
		      : [selector]"m"(selector));
}
