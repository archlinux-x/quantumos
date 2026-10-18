#include "drivers/vga.h"
#include "drivers/keyboard.h"
#include "drivers/serial.h"
#include "gdt.h"
#include "idt.h"
#include "shell.h"

void kernel_main(uint32_t magic, uint32_t addr) {
    (void)addr;

    serial_init();
    vga_init();

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    kprintf("================================================================================\n");
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    kprintf("               Welcome to QuantumOS (x86 32-bit Operating System)               \n");
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    kprintf("================================================================================\n");

    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    kprintf("[*] Booting kernel...\n");

    if (magic == 0x2BADB002) {
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        kprintf("[+] Multiboot magic verified: 0x%x\n", magic);
    } else {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        kprintf("[!] Non-standard bootloader magic: 0x%x\n", magic);
    }

    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    kprintf("[*] Initializing Global Descriptor Table (GDT)... ");
    gdt_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    kprintf("[OK]\n");

    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    kprintf("[*] Initializing Interrupt Descriptor Table (IDT) & PIC... ");
    idt_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    kprintf("[OK]\n");

    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    kprintf("[*] Initializing PS/2 Keyboard Driver... ");
    keyboard_init();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    kprintf("[OK]\n");

    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    kprintf("[*] Enabling hardware interrupts (STI)... ");
    __asm__ volatile ("sti");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    kprintf("[OK]\n\n");

    vga_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    kprintf("System ready! Type 'help' to see available commands.\n\n");

    // Launch the interactive shell
    shell_init();

    // Idle loop waiting for interrupts
    while (1) {
        __asm__ volatile ("hlt");
    }
}
