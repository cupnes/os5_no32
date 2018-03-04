CFLAGS = -Wall -Wextra
CFLAGS += -nostdinc -nostdlib -fno-builtin -fno-common -c
CFLAGS += -Iinclude
CFLAGS += -DX86_64
LDFLAGS = -Map System.map -s -x
LDFLAGS += -T sys_64.ld
OBJS = init_64.o fb.o font.o fbcon.o kbc.o cpu.o excp.o intr.o sys_64.o

kernel_64.bin: $(OBJS)
	ld $(LDFLAGS) -o $@ $+

kernel.bin: $(OBJS)
	ld $(LDFLAGS) -o $@ $+

%.o: %.S
	gcc $(CFLAGS) -o $@ $<
%.o: %.c
	gcc $(CFLAGS) -o $@ $<

clean:
	rm -f *~ *.o *.bin *.dat *.img *.map

.PHONY: clean
