#include "vga.h"
#include "io.h"
#include "serial.h"
#include <stdarg.h>

static volatile uint16_t* const VGA_BUFFER = (volatile uint16_t*)0xB8000;
static size_t vga_row = 0;
static size_t vga_column = 0;
static uint8_t vga_color = 0x0F; // White on black

void vga_update_cursor(void) {
    uint16_t pos = (uint16_t)(vga_row * VGA_WIDTH + vga_column);

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_set_color(vga_color_t fg, vga_color_t bg) {
    vga_color = vga_entry_color(fg, bg);
}

void vga_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            VGA_BUFFER[index] = vga_entry(' ', vga_color);
        }
    }
    vga_row = 0;
    vga_column = 0;
    vga_update_cursor();
}

void vga_init(void) {
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_clear();
}

static void vga_scroll(void) {
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[y * VGA_WIDTH + x] = VGA_BUFFER[(y + 1) * VGA_WIDTH + x];
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        VGA_BUFFER[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', vga_color);
    }
    vga_row = VGA_HEIGHT - 1;
}

void vga_backspace(void) {
    if (vga_column > 0) {
        vga_column--;
        const size_t index = vga_row * VGA_WIDTH + vga_column;
        VGA_BUFFER[index] = vga_entry(' ', vga_color);
        vga_update_cursor();
        serial_puts("\b \b");
    } else if (vga_row > 0) {
        vga_row--;
        vga_column = VGA_WIDTH - 1;
        const size_t index = vga_row * VGA_WIDTH + vga_column;
        VGA_BUFFER[index] = vga_entry(' ', vga_color);
        vga_update_cursor();
        serial_puts("\b \b");
    }
}

void vga_putc(char c) {
    if (c == '\n') {
        vga_column = 0;
        if (++vga_row == VGA_HEIGHT) {
            vga_scroll();
        }
    } else if (c == '\r') {
        vga_column = 0;
    } else if (c == '\t') {
        vga_column = (vga_column + 4) & ~3;
        if (vga_column >= VGA_WIDTH) {
            vga_column = 0;
            if (++vga_row == VGA_HEIGHT) {
                vga_scroll();
            }
        }
    } else if (c == '\b') {
        vga_backspace();
        return;
    } else {
        const size_t index = vga_row * VGA_WIDTH + vga_column;
        VGA_BUFFER[index] = vga_entry((unsigned char)c, vga_color);
        if (++vga_column == VGA_WIDTH) {
            vga_column = 0;
            if (++vga_row == VGA_HEIGHT) {
                vga_scroll();
            }
        }
    }
    vga_update_cursor();
    serial_putc(c);
}

void vga_puts(const char* str) {
    if (!str) return;
    for (size_t i = 0; str[i] != '\0'; i++) {
        vga_putc(str[i]);
    }
}

static void print_uint(uint32_t num, int base, int uppercase) {
    char buf[33];
    int i = 0;
    const char* digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

    if (num == 0) {
        vga_putc('0');
        return;
    }

    while (num > 0) {
        buf[i++] = digits[num % base];
        num /= base;
    }

    while (i > 0) {
        vga_putc(buf[--i]);
    }
}

static void print_int(int32_t num) {
    if (num < 0) {
        vga_putc('-');
        print_uint((uint32_t)(-num), 10, 0);
    } else {
        print_uint((uint32_t)num, 10, 0);
    }
}

void kprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (size_t i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
                case 'c': {
                    char c = (char)va_arg(args, int);
                    vga_putc(c);
                    break;
                }
                case 's': {
                    const char* s = va_arg(args, const char*);
                    vga_puts(s ? s : "(null)");
                    break;
                }
                case 'd':
                case 'i': {
                    int val = va_arg(args, int);
                    print_int(val);
                    break;
                }
                case 'u': {
                    unsigned int val = va_arg(args, unsigned int);
                    print_uint(val, 10, 0);
                    break;
                }
                case 'x': {
                    unsigned int val = va_arg(args, unsigned int);
                    print_uint(val, 16, 0);
                    break;
                }
                case 'X': {
                    unsigned int val = va_arg(args, unsigned int);
                    print_uint(val, 16, 1);
                    break;
                }
                case 'p': {
                    uint32_t val = (uint32_t)va_arg(args, void*);
                    vga_puts("0x");
                    print_uint(val, 16, 0);
                    break;
                }
                case '%': {
                    vga_putc('%');
                    break;
                }
                default:
                    vga_putc('%');
                    vga_putc(fmt[i]);
                    break;
            }
        } else {
            vga_putc(fmt[i]);
        }
    }

    va_end(args);
}
