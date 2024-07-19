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

    const char *panic = "PANIC";

    terminal_clear();
    printf("%s\n", panic);
    vprintf(error, args);
    cursor_disable();

    va_end(args);

    asm("cli");
halt:
    asm("hlt");
    goto halt;
}