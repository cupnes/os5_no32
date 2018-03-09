#include <x86.h>
#include <intr.h>

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
	io_write(IOADR_MPIC_ICW1, 0x11);
	io_write(IOADR_MPIC_ICW2, 0x20);
	io_write(IOADR_MPIC_ICW3, 0x04);
	io_write(IOADR_MPIC_ICW4, 0x01);
	io_write(IOADR_MPIC_OCW1, 0xff);

	/* スレーブPICの初期化 */
	io_write(IOADR_SPIC_ICW1, 0x11);
	io_write(IOADR_SPIC_ICW2, 0x28);
	io_write(IOADR_SPIC_ICW3, 0x02);
	io_write(IOADR_SPIC_ICW4, 0x01);
	io_write(IOADR_SPIC_OCW1, 0xff);
}

void intr_set_mask_master(unsigned char mask)
{
	io_write(IOADR_MPIC_OCW1, mask);
}

unsigned char intr_get_mask_master(void)
{
	return io_read(IOADR_MPIC_OCW1);
}

void intr_set_mask_slave(unsigned char mask)
{
	io_write(IOADR_SPIC_OCW1, mask);
}

unsigned char intr_get_mask_slave(void)
{
	return io_read(IOADR_SPIC_OCW1);
}
