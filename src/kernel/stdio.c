#include <stdio.h>
#include <keyboard.h>
#include <timer.h>

int terminal_x = 80;
int terminal_y = 25;

stdout *terminal_output;
getpos *terminal_getpos;
setpos *terminal_setpos;
clear *terminal_clear;

void set_stdout(stdout *func)
{
    terminal_output = func;
}
void set_getpos(getpos *func)
{
    terminal_getpos = func;
}
void set_setpos(setpos *func)
{
    terminal_setpos = func;
}
void set_clear(clear *func)
{
    terminal_clear = func;
}
void backspace()
{
    int pos = terminal_getpos() - 1;
    int x = pos % terminal_x;
    int y = pos / terminal_x;
    terminal_setpos(x, y);
    putchar(' ');
    terminal_setpos(x, y);
}
char *gets(char *string)
{
    char character;
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
            if (address != string)
            {
                backspace();
                *string-- = '\0';
            }
            break;

        default:
            character = key_to_char(key);
            if (is_key_printable(key))
            {
                putchar(character);
                *string++ = character;
            }
            break;
        }
    }
    newline(); // after Enter pressed
    *string = '\0';
    return string;
}
