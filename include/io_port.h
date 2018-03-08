#ifndef _IO_PORT_H_
#define _IO_PORT_H_

#define outb(value, port)				\
__asm__ ("outb %%al,%%dx"::"a" (value),"d" (port))

#define inb(port) ({ \
unsigned char _v; \
__asm__ volatile ("inb %%dx,%%al":"=a" (_v):"d" (port)); \
_v; \
})

#endif /* _IO_PORT_H_ */
