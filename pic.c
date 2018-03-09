#include <pic.h>
#include <x86.h>

#define MPIC_ICW1_ADDR	0x0020
#define MPIC_OCW2_ADDR	0x0020
#define MPIC_OCW3_ADDR	0x0020
#define MPIC_ICW2_ADDR	0x0021
#define MPIC_ICW3_ADDR	0x0021
#define MPIC_ICW4_ADDR	0x0021
#define MPIC_OCW1_ADDR	0x0021
#define MPIC_IMR_ADDR	0x0021
#define SPIC_ICW1_ADDR	0x00a0
#define SPIC_OCW2_ADDR	0x00a0
#define SPIC_ICW2_ADDR	0x00a1
#define SPIC_ICW3_ADDR	0x00a1
#define SPIC_ICW4_ADDR	0x00a1
#define SPIC_OCW1_ADDR	0x00a1
#define SPIC_IMR_ADDR	0x00a1

#define PIC_OCW2_BIT_MANUAL_EOI	0x60

#define INTR_NO_BASE_MASTER	32
#define INTR_NO_BASE_SLAVE	40

unsigned long long pic_intr_counter = 0;

void pic_init(void)
{
	/* マスタPICの初期化 */
	io_write(MPIC_ICW1_ADDR, 0x11);
	io_write(MPIC_ICW2_ADDR, INTR_NO_BASE_MASTER);
	io_write(MPIC_ICW3_ADDR, 0x04);
	io_write(MPIC_ICW4_ADDR, 0x01);
	io_write(MPIC_OCW1_ADDR, 0xff);

	/* スレーブPICの初期化 */
	io_write(SPIC_ICW1_ADDR, 0x11);
	io_write(SPIC_ICW2_ADDR, INTR_NO_BASE_SLAVE);
	io_write(SPIC_ICW3_ADDR, 0x02);
	io_write(SPIC_ICW4_ADDR, 0x01);
	io_write(SPIC_OCW1_ADDR, 0xff);
}

void enable_pic_intr(unsigned char intr_no)
{
	/* マスタ/スレーブのIR番号とレジスタIOアドレス取得 */
	unsigned char ir_no;
	unsigned short imr_addr;
	unsigned short ocw1_addr;
	if (intr_no < INTR_NO_BASE_SLAVE) {
		ir_no = intr_no - INTR_NO_BASE_MASTER;
		imr_addr = MPIC_IMR_ADDR;
		ocw1_addr = MPIC_OCW1_ADDR;
	} else {
		ir_no = intr_no - INTR_NO_BASE_SLAVE;
		imr_addr = SPIC_IMR_ADDR;
		ocw1_addr = SPIC_OCW1_ADDR;
	}

	/* 既にマスク解除済みならば何もせずreturn */
	unsigned char ir_bit = 1U << ir_no;
	unsigned char mask = io_read(imr_addr);
	if (!(ir_bit & mask))
		return;

	/* マスク解除 */
	io_write(ocw1_addr, mask - ir_bit);
}

void set_pic_eoi(unsigned char intr_no)
{
	/* マスタ/スレーブのIR番号とレジスタIOアドレス取得 */
	unsigned char ir_no;
	unsigned short ocw2_addr;
	if (intr_no < INTR_NO_BASE_SLAVE) {
		ir_no = intr_no - INTR_NO_BASE_MASTER;
		ocw2_addr = MPIC_OCW2_ADDR;
	} else {
		ir_no = intr_no - INTR_NO_BASE_SLAVE;
		ocw2_addr = SPIC_OCW2_ADDR;
	}

	/* EOI設定 */
	io_write(ocw2_addr, PIC_OCW2_BIT_MANUAL_EOI | ir_no);
}
