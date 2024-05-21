#include <keyboard.h>
#include <stdio.h>
#include <interrupts.h>
#include <memory.h>
#include <io.h>

#define KB_BUFFER_SIZE 128

#define KEY_PRESSED 1
#define KEY_RELEASED 2

char kb_keys[] = " " // zero
                 "?1234567890-=??"
                 "qwertyuiop[]??"
                 "asdfghjkl;'`?\\"
                 "zxcvbnm,./?*? ?"
                 "????????????" // f1-f10
                 "??7894561230."
                 "   "   // empty
                 "??"    // f11, f12
                 "    "; // empty

uint8_t kb_buffer[KB_BUFFER_SIZE];
uint8_t key_scancode;

uint8_t char_to_scancode(char character)
{
    for (int i = 0; i < 94; i++)
    {
        if (kb_keys[i] == character)
            return i;
    }
    return 0;
}
bool is_key_pressed(enum Key scancode)
{
    return kb_buffer[scancode] == KEY_PRESSED;
}
bool is_key_released(enum Key scancode)
{
    return kb_buffer[scancode] == KEY_RELEASED;
}
bool is_key_printable(keyboard_key_t key)
{
    return kb_keys[key.scancode] != '?';
}
bool is_key_letter(keyboard_key_t key)
{
    return kb_keys[key.scancode] >= 'a' && kb_keys[key.scancode] <= 'z';
}
keyboard_key_t keyboard_input()
{
    key_scancode = 0;
    io_wait();

    keyboard_key_t key = (keyboard_key_t){0};

    key.ctrl = is_key_pressed(Ctrl);
    key.shift = is_key_pressed(LShift) || is_key_pressed(RShift);
    key.alt = is_key_pressed(Alt);
    key.caps_lock = is_key_pressed(CapsLock);
    key.num_lock = is_key_pressed(NumLock);
    key.scroll_lock = is_key_pressed(ScrollLock);

    key.scancode = key_scancode;
    key.pressed = is_key_pressed((enum Key)key_scancode);
    
    if (is_key_printable(key))
    {
        if (is_key_letter(key) && key.shift)
            key.character = kb_keys[key_scancode] + 'A' - 'a';
        else
            key.character = kb_keys[key_scancode];
    }
    
    if (kb_buffer[key_scancode] == KEY_RELEASED)
        kb_buffer[key_scancode] = 0;

    return key;
}
void kb_handler()
{
    key_scancode = 0;

    outb(0x20, 0x20);
    uint8_t scancode = inb(0x60);
    bool pressed = scancode < 0x80;
    if (!pressed)
        scancode -= 0x80;
    kb_buffer[scancode] = pressed ? KEY_PRESSED : KEY_RELEASED;
    key_scancode = pressed ? scancode : 0;
}
void keyboard_init()
{
    set_irq_handler(33, kb_handler);

    outb(0x60, 0xF3);
    outb(0x60, 0b01011111);
}
