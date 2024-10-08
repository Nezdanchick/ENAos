#include <stdio.h>
#include <stdarg.h>
#include <keyboard.h>
#include <timer.h>
#include <serial.h>

int terminal_x = 0;
int terminal_y = 0;
int terminal_width = 80;
int terminal_height = 25;

void stdio_stub() {}
void stdio_stub_write(__attribute__((unused)) char *string) {}
void stdio_stub_setpos(
    __attribute__((unused)) int x,
    __attribute__((unused)) int y) {}

backspace *terminal_backspace = stdio_stub;
clear *terminal_clear = stdio_stub;
scroll *terminal_scroll = stdio_stub;
stdout *terminal_output = stdio_stub_write;
setpos *cursor_setpos = stdio_stub_setpos;

// set std functions
void set_backspace(backspace *func)
{
    terminal_backspace = func;
}
void set_stdout(stdout *func)
{
    terminal_output = func;
}
void set_cursor_setpos(setpos *func)
{
    cursor_setpos = func;
}
void set_scroll(scroll *func)
{
    terminal_scroll = func;
}
void set_clear(clear *func)
{
    terminal_clear = func;
}

// terminal positioning functions
int terminal_getpos()
{
    terminal_check_position();
    return terminal_x + terminal_y * terminal_width;
}
void terminal_check_position()
{
    if (terminal_x >= terminal_width)
    {
        terminal_y += terminal_x / terminal_width;
        terminal_x = terminal_x % terminal_width;
    }
    if (terminal_y >= terminal_height)
        terminal_scroll();
}
void terminal_setpos(int collumn, int row)
{
    terminal_x = collumn;
    terminal_y = row;
    terminal_check_position();
}

// terminal input
char *gets(char *string)
{
    char *address = string;

    keyboard_key_t key = (keyboard_key_t){0};
    while (key.scancode != Enter)
    {
        key = keyboard_input();
        switch (key.scancode)
        {
        case 0:
            break;
        case Backspace:
            if (address < string)
            {
                terminal_backspace();
                *string-- = '\0';
            }
            break;

        default:
            if (is_key_printable(key))
            {
                printf("%c", key.character);
                *string++ = key.character;
            }
            break;
        }
    }
    // after Enter pressed
    terminal_x = 0;
    terminal_y++;
    terminal_check_position();
    *string = '\0';
    return string;
}

// terminal output
void printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vprintf(fmt, args);

    va_end(args);
}
