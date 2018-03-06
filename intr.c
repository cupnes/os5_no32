#include <cpu.h>
#include <intr.h>
#include <io_port.h>

#define MAX_IDT	256

unsigned long long idtr[2];

#ifndef OS5_INTR
#define MAX_INTR_NO	256
#define DESC_TYPE_INTR	14

struct interrupt_descriptor {
	unsigned short offset_00_15;
	unsigned short segment_selector;
	unsigned short ist	: 3;
	unsigned short _zero1	: 5;
	unsigned short type	: 4;
	unsigned short _zero2	: 1;
	unsigned short dpl	: 2;
	unsigned short p	: 1;
	unsigned short offset_31_16;
	unsigned int   offset_63_32;
	unsigned int   _reserved;
};
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
}
#else
unsigned long long idt[MAX_IDT * 2];

void intr_init(void)
{
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

	idtr[0] = ((unsigned long long)idt << 16) | (MAX_IDT * 16 - 1);
	idtr[1] = ((unsigned long long)idt >> 48);
	__asm__ ("lidt idtr");
}
#endif

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

#ifdef OS5_INTR
void intr_set_handler(unsigned char intr_num, unsigned long long handler_addr)
{
	unsigned int intr_dscr[3];
	unsigned int *idt_ptr;
	unsigned char i;

	idt_ptr = (unsigned int *)idt;

	intr_dscr[0] = (SS_KERNEL_CODE << 16) | (handler_addr & 0x0000ffff);
	intr_dscr[1] = (handler_addr & 0xffff0000) | 0x00008e00;
	intr_dscr[2] = handler_addr >> 32;

	idt_ptr += intr_num * 4;
	for (i = 0; i < 3; i++)
		*idt_ptr++ = intr_dscr[i];
}
#endif
