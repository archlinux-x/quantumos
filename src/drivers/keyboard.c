#include "keyboard.h"
#include "io.h"
#include "kernel/idt.h"
#include "kernel/pic.h"

static key_callback_t active_callback = 0;
static int shift_pressed = 0;
static int caps_lock = 0;

static const char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t',
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, /* Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, /* Left shift */
  '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0, /* Right shift */
  '*',
    0, /* Alt */
  ' ', /* Space */
    0, /* Caps lock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* F1-F10 */
    0, /* Num lock */
    0, /* Scroll lock */
    0, /* Home */
    0, /* Up */
    0, /* Page Up */
  '-',
    0, /* Left */
    0,
    0, /* Right */
  '+',
    0, /* End */
    0, /* Down */
    0, /* Page Down */
    0, /* Insert */
    0, /* Delete */
    0, 0, 0,
    0, /* F11 */
    0  /* F12 */
};

static const char kbd_us_shift[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
  '\t',
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, /* Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~',
    0, /* Left shift */
  '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0, /* Right shift */
  '*',
    0, /* Alt */
  ' ', /* Space */
    0, /* Caps lock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* F1-F10 */
    0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static void keyboard_handler(registers_t* regs) {
    (void)regs;
    uint8_t scancode = inb(0x60);

    // Key release
    if (scancode & 0x80) {
        uint8_t released_code = scancode & 0x7F;
        if (released_code == 0x2A || released_code == 0x36) {
            shift_pressed = 0;
        }
        return;
    }

    // Key press
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return;
    }
    if (scancode == 0x3A) {
        caps_lock = !caps_lock;
        return;
    }

    if (scancode < sizeof(kbd_us)) {
        char c = shift_pressed ? kbd_us_shift[scancode] : kbd_us[scancode];

        // Apply caps lock logic for letters
        if (caps_lock && c >= 'a' && c <= 'z') {
            c -= 32;
        } else if (caps_lock && c >= 'A' && c <= 'Z') {
            c += 32;
        }

        if (c != 0 && active_callback != 0) {
            active_callback(c);
        }
    }
}

void keyboard_set_callback(key_callback_t cb) {
    active_callback = cb;
}

void keyboard_init(void) {
    register_interrupt_handler(33, keyboard_handler); // IRQ 1 -> INT 33
    pic_clear_mask(1); // Unmask IRQ 1
}
