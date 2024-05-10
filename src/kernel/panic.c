#include <panic.h>

#include <stdarg.h>
#include <string.h>
#include <screen.h>
#include <cursor.h>
#include <stdio.h>

__attribute__((noreturn)) void panic(const char *error, ...)
{
    va_list args;
    va_start(args, error);

    terminal_clear();
    screen_set_color(SCREEN_COLOR_RED, SCREEN_COLOR_BLACK);
    terminal_setpos((terminal_x - strlen(error)) / 2, terminal_y / 2);
    vprintf(error, args);
    disable_cursor();

    va_end(args);
halt:
    asm("hlt");
    goto halt;
}