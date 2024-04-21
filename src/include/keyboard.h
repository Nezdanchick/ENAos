#pragma once
#include <stdint.h>
#include <stdbool.h>

enum Key
{
    Ctrl = 0x1D,
    LShift = 0x2A,
    RShift = 0x36,
    Alt = 0x38,
    CapsLock = 0x3A,
    NumLock = 0x45,
    ScrollLock = 0x46,

    Escape = 0x01,
    Enter = 0x1C,
    Backspace = 0x0E,
};

typedef struct {
    bool shift : 1;
    bool alt : 1;
    bool ctrl : 1;
    bool fn : 1;

    bool caps_lock : 1;
    bool scroll_lock : 1;
    bool num_lock : 1;

    // is key pressed or released
    bool pressed : 1;

    uint8_t scancode;
    uint8_t character;
} __attribute__((__packed__)) keyboard_key_t;

extern void keyboard_init();
extern keyboard_key_t keyboard_input();
extern char key_to_char(keyboard_key_t key);
uint8_t char_to_scancode(char character);
extern bool is_key_pressed(enum Key scancode);
extern bool is_key_released(enum Key scancode);
extern bool is_key_printable(keyboard_key_t key);
