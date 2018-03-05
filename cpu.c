#include <cpu.h>
#include <asm/cpu.h>

unsigned long long gdtr[2];

#ifndef OS5
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
	/* asm volatile ("movw $2*8, %ax\n" */
	/* 	      "movw %ax, %ds\n" */
	/* 	      "movw %ax, %ss\n"); */
	asm volatile ("movw $2*8, %ax\n"
		      "movw %ax, %ds\n"
		      "movw %ax, %es\n"
		      "movw %ax, %fs\n"
		      "movw %ax, %gs\n"
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
#else
struct segment_descriptor gdt[GDT_SIZE];

void gdt_init(void)
{
	unsigned char i;

	for (i = 0; i < GDT_SIZE; i++)
		gdt[i].a = gdt[i].b = 0;

	gdt_set(1, 0, 0xfffff, 1, 0, 1, 0, GDT_S_CODE_OR_DATA,
		GDT_TYPE_CODE_ER);
	gdt_set(2, 0, 0xfffff, 1, 1, 0, 0, GDT_S_CODE_OR_DATA,
		GDT_TYPE_DATA_RW);

	gdtr[0] = ((unsigned long long)gdt << 16) | (GDT_SIZE * 8 - 1);
	gdtr[1] = ((unsigned long long)gdt >> 48);

	__asm__ ("lgdt	gdtr\n"
		 "mov	$2*8, %ax\n"
		 "mov	%ax, %ds\n"
		 "mov	%ax, %es\n"
		 "mov	%ax, %fs\n"
		 "mov	%ax, %gs\n"
		 "mov	%ax, %ss\n");

	unsigned short selector = 8;
	unsigned long long dummy;
	__asm__ ("pushq %[selector];"
		 "leaq ret_label(%%rip), %[dummy];"
		 "pushq %[dummy];"
		 "lretq;"
		 "ret_label:"
		 : [dummy]"=r"(dummy)
		 : [selector]"m"(selector));
}

void gdt_set(unsigned int idx, unsigned int base, unsigned int limit,
	     unsigned char g, unsigned char d, unsigned char l,
	     unsigned char dpl, unsigned char s, unsigned char type)
{
	gdt[idx].a = gdt[idx].b = 0;

	gdt[idx].limit0 = limit & 0x0000ffff;
	gdt[idx].limit1 = (limit & 0x000f0000) >> 16;

	gdt[idx].base0 = base & 0x0000ffff;
	gdt[idx].base1 = (base & 0x00ff0000) >> 16;
	gdt[idx].base2 = (base & 0xff000000) >> 24;

	gdt[idx].g = g;
	gdt[idx].d = d;
	gdt[idx].l = l;

	gdt[idx].dpl = dpl;

	gdt[idx].s = s;
	gdt[idx].type = type;
	gdt[idx].p = 1;
}
#endif
