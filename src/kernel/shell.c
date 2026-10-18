#include "shell.h"
#include "drivers/vga.h"
#include "drivers/io.h"
#include "drivers/keyboard.h"

#define BUFFER_SIZE 256

static char input_buffer[BUFFER_SIZE];
static int buffer_index = 0;

static int kstrcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

static int kstrncmp(const char* s1, const char* s2, int n) {
    while (n > 0 && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

__attribute__((unused)) static int kstrlen(const char* s) {
    int len = 0;
    while (s[len]) len++;
    return len;
}

static void print_prompt(void) {
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    kprintf("quantumos> ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

static void cmd_help(void) {
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    kprintf("Available commands:\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    kprintf("  help             - Show this help menu\n");
    kprintf("  clear            - Clear the terminal screen\n");
    kprintf("  about            - Information about QuantumOS\n");
    kprintf("  echo <text>      - Print text to screen\n");
    kprintf("  color <0-15>     - Change text foreground color\n");
    kprintf("  reboot           - Reboot the system\n");
    kprintf("  halt             - Stop the CPU\n");
}

static void cmd_about(void) {
    vga_set_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    kprintf("===========================================\n");
    kprintf("        QuantumOS v0.1 (x86 32-bit)        \n");
    kprintf("===========================================\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    kprintf("Architecture : x86 (IA-32 Protected Mode)\n");
    kprintf("Boot Protocol: Multiboot (GRUB / QEMU)\n");
    kprintf("Drivers      : VGA 80x25 Text, PS/2 Keyboard, 8259 PIC\n");
    kprintf("Features     : Custom GDT, IDT with 32 exceptions & IRQs\n");
    kprintf("Author       : neko\n");
}

static void cmd_color(const char* arg) {
    while (*arg == ' ') arg++;
    if (*arg >= '0' && *arg <= '9') {
        int col = *arg - '0';
        if (*(arg + 1) >= '0' && *(arg + 1) <= '9') {
            col = col * 10 + (*(arg + 1) - '0');
        }
        if (col >= 0 && col <= 15) {
            vga_set_color((vga_color_t)col, VGA_COLOR_BLACK);
            kprintf("Color changed to %d\n", col);
            return;
        }
    }
    kprintf("Usage: color <0-15>\n");
}

static void cmd_reboot(void) {
    kprintf("Rebooting...\n");
    // Pulse keyboard controller reset line
    uint8_t temp;
    do {
        temp = inb(0x64);
        if (temp & 1) inb(0x60);
    } while (temp & 2);
    outb(0x64, 0xFE);
    __asm__ volatile ("hlt");
}

static void cmd_halt(void) {
    kprintf("Halting CPU. It is now safe to close the emulator.\n");
    __asm__ volatile ("cli; hlt");
}

static void execute_command(char* cmd) {
    // Strip leading spaces
    while (*cmd == ' ') cmd++;
    if (*cmd == '\0') return;

    if (kstrcmp(cmd, "help") == 0) {
        cmd_help();
    } else if (kstrcmp(cmd, "clear") == 0) {
        vga_clear();
    } else if (kstrcmp(cmd, "about") == 0) {
        cmd_about();
    } else if (kstrncmp(cmd, "echo ", 5) == 0) {
        kprintf("%s\n", cmd + 5);
    } else if (kstrcmp(cmd, "echo") == 0) {
        kprintf("\n");
    } else if (kstrncmp(cmd, "color", 5) == 0) {
        cmd_color(cmd + 5);
    } else if (kstrcmp(cmd, "reboot") == 0) {
        cmd_reboot();
    } else if (kstrcmp(cmd, "halt") == 0) {
        cmd_halt();
    } else {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        kprintf("Unknown command: '%s'. Type 'help' for commands.\n", cmd);
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    }
}

void shell_handle_key(char c) {
    if (c == '\n') {
        vga_putc('\n');
        input_buffer[buffer_index] = '\0';
        execute_command(input_buffer);
        buffer_index = 0;
        print_prompt();
    } else if (c == '\b') {
        if (buffer_index > 0) {
            buffer_index--;
            vga_putc('\b');
        }
    } else if (c >= ' ' && c <= '~') {
        if (buffer_index < BUFFER_SIZE - 1) {
            input_buffer[buffer_index++] = c;
            vga_putc(c);
        }
    }
}

void shell_init(void) {
    buffer_index = 0;
    keyboard_set_callback(shell_handle_key);
    print_prompt();
}
