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

    const char *p = "PANIC: ";

    terminal_clear();
    int msglen = strlen(error) + strlen(p);
    terminal_setpos((terminal_width - msglen % terminal_width) / 2, (terminal_height - msglen / terminal_width) / 2);
    printf(p);
    vprintf(error, args);
    cursor_disable();

    va_end(args);

    asm("cli");
halt:
    asm("hlt");
    goto halt;
}