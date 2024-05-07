#include <panic.h>
#include <stdarg.h>
#include <screen.h>
#include <cursor.h>
#include <stdio.h>

void panic(char *error, ...)
{
    va_list args;
    va_start(args, error);

    terminal_clear();
    screen_set_color(SCREEN_COLOR_RED, SCREEN_COLOR_BLACK);
    printf(error, args);
    disable_cursor();

    va_end(args);
    asm("cli");
    asm("hlt");
halt:
    goto halt;
}