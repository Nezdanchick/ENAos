#pragma once

#include <stdint.h>
#include <stdbool.h>

enum Key
{
    // control keys
    Escape = 0x01,
    Backspace = 0x0E,
    Tab = 0x0F,
    Enter = 0x1C,
    LCtrl = 0x1D,
    LShift = 0x2A,
    RShift = 0x36,
    LAlt = 0x38,
    Space = 0x39,
    CapsLock = 0x3A,
    NumLock = 0x45,
    ScrollLock = 0x46,

    // top number row
    Num1 = 0x02, Num2 = 0x03, Num3 = 0x04, Num4 = 0x05,
    Num5 = 0x06, Num6 = 0x07, Num7 = 0x08, Num8 = 0x09,
    Num9 = 0x0A, Num0 = 0x0B,
    Minus = 0x0C, Equal = 0x0D,

    // letters
    Q = 0x10, W = 0x11, E = 0x12, R = 0x13, T = 0x14, Y = 0x15, U = 0x16, I = 0x17, O = 0x18, P = 0x19,
    LeftBracket = 0x1A, RightBracket = 0x1B,
    A = 0x1E, S = 0x1F, D = 0x20, F = 0x21, G = 0x22, H = 0x23, J = 0x24, K = 0x25, L = 0x26,
    Semicolon = 0x27, SingleQuote = 0x28, BackTick = 0x29, Backslash = 0x2B,
    Z = 0x2C, X = 0x2D, C = 0x2E, V = 0x2F, B = 0x30, N = 0x31, M = 0x32,
    Comma = 0x33, Period = 0x34, Slash = 0x35,

    // function keys
    F1 = 0x3B, F2 = 0x3C, F3 = 0x3D, F4 = 0x3E, F5 = 0x3F,
    F6 = 0x40, F7 = 0x41, F8 = 0x42, F9 = 0x43, F10 = 0x44,
    F11 = 0x57, F12 = 0x58,

    // numpad
    // act as arrows when numlock is off
    KeypadStar = 0x37,
    Keypad7_Home = 0x47, Keypad8_Up = 0x48, Keypad9_PgUp = 0x49, KeypadMinus = 0x4A,
    Keypad4_Left = 0x4B, Keypad5_Clear = 0x4C, Keypad6_Right = 0x4D, KeypadPlus = 0x4E,
    Keypad1_End = 0x4F, Keypad2_Down = 0x50, Keypad3_PgDn = 0x51,
    Keypad0_Insert = 0x52, KeypadPeriod_Delete = 0x53,

    // extended keys
    // these send an 0xe0 prefix first
    RCtrl = 0x1D,       // e0 1d
    RAlt = 0x38,        // e0 38
    Home = 0x47,        // e0 47
    UpArrow = 0x48,     // e0 48
    PageUp = 0x49,      // e0 49
    LeftArrow = 0x4B,   // e0 4b
    RightArrow = 0x4D,  // e0 4d
    End = 0x4F,         // e0 4f
    DownArrow = 0x50,   // e0 50
    PageDown = 0x51,    // e0 51
    Insert = 0x52,      // e0 52
    Delete = 0x53,      // e0 53
    LSuper = 0x5B,      // e0 5b (left windows/super)
    RSuper = 0x5C,      // e0 5c (right windows/super)
    Apps = 0x5D,        // e0 5d (context menu)
};

typedef struct
{
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

extern void init_keyboard();
extern keyboard_key_t keyboard_input();
extern void wait_for_scancode(enum Key  scancode);
extern int char_to_scancode(char character);
extern bool is_key_pressed(enum Key scancode);
extern bool is_key_released(enum Key scancode);
extern bool is_key_printable(keyboard_key_t key);
extern bool is_key_number(keyboard_key_t key);
