#include <panic.h>
#include <stdarg.h>
#include <string.h>
#include <cursor.h>
#include <stdio.h>

__attribute__((sysv_abi, noinline, noreturn)) void _panic(const char *file, int line, const char *func, const char *msg, ...)
{
    __asm__ volatile("cli");

    printf("\n!!! KERNEL PANIC !!!\n\n");
    printf("PATH       %s:%d\n", file, line);
    printf("FUNCTION   %s\n", func);
    printf("MESSAGE    ");

    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);

    printf("\n");

    while (true)
    {
        __asm__ volatile("hlt");
    }
}