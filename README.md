# QuantumOS — Operating System in C (x86 32-bit)

A minimalistic educational operating system written in C and x86 Assembly (NASM), supporting the Multiboot specification (GRUB) and running in QEMU.

---

## 📁 Project Structure

```text
quantumos/
├── Makefile                    # Build script (kernel ELF & ISO)
├── linker.ld                  # Linker script (links kernel at 1MB)
├── isodir/
│   └── boot/
│       └── grub/
│           └── grub.cfg       # GRUB menu configuration
└── src/
    ├── arch/
    │   └── i386/
    │       ├── boot.asm       # Multiboot header, stack setup, and _start entry point
    │       ├── gdt_flush.asm  # Reloads CS/DS segment registers
    │       └── interrupts.asm # Assembly stubs for interrupts (ISR 0-31, IRQ 0-15)
    ├── drivers/
    │   ├── io.h               # I/O port operations (inb, outb, io_wait)
    │   ├── vga.h / vga.c      # VGA Text Mode (80x25), cursor management, scrolling, kprintf
    │   ├── keyboard.h / keyboard.c # PS/2 keyboard driver (scancode handling, Shift, Caps Lock)
    │   └── serial.h / serial.c # COM1 serial port driver (for debugging)
    └── kernel/
        ├── gdt.h / gdt.c      # Global Descriptor Table (kernel and user segments)
        ├── idt.h / idt.c      # Interrupt Descriptor Table (CPU exceptions and IRQ handling)
        ├── pic.h / pic.c      # 8259 PIC interrupt controller remapping
        ├── shell.h / shell.c  # Interactive command-line shell (help, clear, echo, reboot, halt)
        └── kernel.c           # Main kernel entry point (kernel_main)
🚀 Building and Running

Run all commands from the root directory of the project (/mnt/c/Users/neko/Documents/quantumos):
1. Build the kernel and bootable ISO image
Bash

make

This produces:

    quantumos.bin / quantum.bin — ELF kernel binary.

    quantumos.iso / quantum.iso — Bootable ISO image with GRUB.

2. Run in QEMU (using ISO)
Bash

make run

Or run directly via QEMU:
Bash

qemu-system-i386 -cdrom quantumos.iso

3. Quick boot without ISO (Direct Kernel Boot)
Bash

make run-kernel

4. Clean build artifacts
Bash

make clean

💻 Built-in Shell Commands

Once booted, an interactive command-line terminal is available:

    help — Show list of available commands

    about — Display system and architecture information

    clear — Clear the VGA screen

    echo <text> — Output text to the screen

    color <0-15> — Change console font color

    reboot — Reboot the virtual machine

    halt — Halt the processor (cli; hlt)
