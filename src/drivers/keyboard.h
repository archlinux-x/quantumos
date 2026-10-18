#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

typedef void (*key_callback_t)(char c);

void keyboard_init(void);
void keyboard_set_callback(key_callback_t cb);

#endif // KEYBOARD_H
