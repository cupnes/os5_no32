#include <x86.h>
#include <intr.h>
#include <kbc.h>
#include <fbcon.h>
#include <pic.h>

const char keymap[] = {
	0x00, ASCII_ESC, '1', '2', '3', '4', '5', '6',
	'7', '8', '9', '0', '-', '^', ASCII_BS, ASCII_HT,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
	'o', 'p', '@', '[', '\n', 0x00, 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
	':', 0x00, 0x00, ']', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', 0x00, '*',
	0x00, ' ', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, '7',
	'8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, '_', 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, '\\', 0x00, 0x00
};

void do_ir_keyboard(void)
{
	unsigned char status;
	status = io_read(IOADR_KBC_STATUS);
	if (status & IOADR_KBC_STATUS_BIT_OBF) {
		unsigned char keycode = io_read(IOADR_KBC_DATA);
		if (!(keycode & IOADR_KBC_DATA_BIT_BRAKE)) {
			char c = keymap[keycode];
			if (('a' <= c) && (c <= 'z'))
				c = c - 'a' + 'A';
			else if (c == '\n')
				putc('\r');
			putc(c);
		}
	}
	io_write(IOADR_MPIC_OCW2, IOADR_MPIC_OCW2_BIT_MANUAL_EOI | INTR_IR_KB);
}

unsigned char get_keydata_noir(void)
{
	while (!(io_read(IOADR_KBC_STATUS) & IOADR_KBC_STATUS_BIT_OBF));
	return io_read(IOADR_KBC_DATA);
}

unsigned char get_keycode_pressed(void)
{
	unsigned char keycode;
	while ((keycode = get_keydata_noir()) & IOADR_KBC_DATA_BIT_BRAKE);
	return keycode & ~IOADR_KBC_DATA_BIT_BRAKE;
}

void kbc_init(void)
{
	set_intr_desc(INTR_NUM_KB, keyboard_handler);
	enable_pic_intr(INTR_NUM_KB);
}
