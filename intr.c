#include <x86.h>
#include <intr.h>
#include <io_port.h>

#define MAX_IDT	256

unsigned long long idtr[2];

#define MAX_INTR_NO	256
#define DESC_TYPE_INTR	14

struct interrupt_descriptor idt[MAX_INTR_NO];

void default_handler(void);

void set_intr_desc(unsigned char intr_no, void *handler)
{
	idt[intr_no].offset_00_15 = (unsigned long long)handler;
	idt[intr_no].segment_selector = SS_KERNEL_CODE;
	idt[intr_no].type = DESC_TYPE_INTR;
	idt[intr_no].p = 1;
	idt[intr_no].offset_31_16 = (unsigned long long)handler >> 16;
	idt[intr_no].offset_63_32 = (unsigned long long)handler >> 32;
}

void intr_init(void)
{
	int i;
	for (i = 0; i < MAX_INTR_NO; i++)
		set_intr_desc(i, default_handler);

	idtr[0] = ((unsigned long long)idt << 16) | (sizeof(idt) - 1);
	idtr[1] = ((unsigned long long)idt >> 48);
	__asm__ ("lidt idtr");

	/* マスタPICの初期化 */
	outb_p(0x11, IOADR_MPIC_ICW1);
	outb_p(0x20, IOADR_MPIC_ICW2);
	outb_p(0x04, IOADR_MPIC_ICW3);
	outb_p(0x01, IOADR_MPIC_ICW4);
	outb_p(0xff, IOADR_MPIC_OCW1);

	/* スレーブPICの初期化 */
	outb_p(0x11, IOADR_SPIC_ICW1);
	outb_p(0x28, IOADR_SPIC_ICW2);
	outb_p(0x02, IOADR_SPIC_ICW3);
	outb_p(0x01, IOADR_SPIC_ICW4);
	outb_p(0xff, IOADR_SPIC_OCW1);
}

void intr_set_mask_master(unsigned char mask)
{
	outb_p(mask, IOADR_MPIC_OCW1);
}

unsigned char intr_get_mask_master(void)
{
	return inb_p(IOADR_MPIC_OCW1);
}

void intr_set_mask_slave(unsigned char mask)
{
	outb_p(mask, IOADR_SPIC_OCW1);
}

unsigned char intr_get_mask_slave(void)
{
	return inb_p(IOADR_SPIC_OCW1);
}
