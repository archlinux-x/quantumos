CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -fno-builtin -nostdlib -Isrc
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

C_SRCS = $(wildcard src/kernel/*.c) $(wildcard src/drivers/*.c)
ASM_SRCS = $(wildcard src/arch/i386/*.asm)

OBJS = $(ASM_SRCS:.asm=.o) $(C_SRCS:.c=.o)

all: quantumos.bin quantumos.iso quantum.bin quantum.iso

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

quantumos.bin: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

quantumos.iso: quantumos.bin isodir/boot/grub/grub.cfg
	mkdir -p isodir/boot
	cp quantumos.bin isodir/boot/quantumos.bin
	grub-mkrescue -o quantumos.iso isodir

quantum.bin: quantumos.bin
	cp quantumos.bin quantum.bin

quantum.iso: quantumos.iso
	cp quantumos.iso quantum.iso

run: quantumos.iso
	qemu-system-i386 -cdrom quantumos.iso

run-kernel: quantumos.bin
	qemu-system-i386 -kernel quantumos.bin

clean:
	rm -f $(OBJS) quantumos.bin quantumos.iso quantum.bin quantum.iso isodir/boot/quantumos.bin isodir/boot/quantum.bin myos.bin myos.iso isodir/boot/myos.bin

.PHONY: all run run-kernel clean
