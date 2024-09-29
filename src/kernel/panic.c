#include <panic.h>

#include <stdarg.h>
#include <string.h>
#include <screen.h>
#include <cursor.h>
#include <stdio.h>

__attribute__((noreturn)) void _panic(const char *error, ...)
{
    va_list args;
    va_start(args, error);
    
    terminal_clear();
    printf("!!!PANIC!!!\n\n");  // TODO add useful info like registers
    vprintf(error, args);

    va_end(args);

    asm("cli");
halt:
    asm("hlt");
    goto halt;
}