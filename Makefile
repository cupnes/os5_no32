CFLAGS = -Wall -Wextra
CFLAGS += -nostdinc -nostdlib -fno-builtin -fno-common -c
CFLAGS += -Iinclude
LDFLAGS = -Map System.map -s -x
LDFLAGS += -T kernel.ld
OBJS = main.o fb.o font.o fbcon.o kbc.o x86.o excp.o intr.o handler.o

kernel_64.bin: $(OBJS)
	ld $(LDFLAGS) -o $@ $+

%.o: %.s
	gcc $(CFLAGS) -o $@ $<
%.o: %.c
	gcc $(CFLAGS) -o $@ $<

clean:
	rm -f *~ *.o *.bin *.dat *.img *.map

.PHONY: clean
