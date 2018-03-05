#include <excp.h>
#include <fbcon.h>

void do_exception(void)
{
	puts("exception\r\n");
	while (1);
}

void do_page_fault(unsigned int error_code, unsigned int address)
{
	(void)error_code;
	(void)address;
	puts("page fault\r\n");
	while (1);
}
