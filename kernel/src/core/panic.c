#include <stdint.h>
#include <stddef.h>
#include "panic.h"
#include "serial.h"

__attribute__((sysv_abi, noinline, noreturn))
void _panic(const char *file, int line, const char *func, const char *msg, const cpu_context_t *context)
{
    __asm__ volatile("cli");

    serial_formatted("\r\n!!! KERNEL PANIC !!!\r\n");
    serial_formatted("PATH:     %s:%d\r\n", file, line);
    serial_formatted("FUNCTION: %s\r\n", func);
    serial_formatted("MESSAGE:  %s\r\n", msg);

    if (context != NULL)
    {
        uint64_t cr2, cr3;
        __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
        __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));

        serial_formatted("CPU:      RIP=%x, RSP=%x, RFLAGS=%x, CR2=%x, CR3=%x\r\n",
                         context->rip, context->rsp, context->rflags, cr2, cr3);
    }

    while (1)
    {
        __asm__ volatile("hlt");
    }
}
