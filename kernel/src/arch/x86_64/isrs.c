#include <stddef.h>
#include "isrs.h"
#include "screen.h"
#include "serial.h"

static char *exception_name[] = {
    "Division Error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved",
    "Triple Fault",
    "FPU Error Interrupt",
    "Reserved",
    "Reserved",
    "Reserved",
};

static irq_handler *irq_handlers[256] = {0};
static exception_handler *exception_handlers[32] = {0};

void set_irq_handler(uint8_t interrupt, irq_handler *handler)
{
    irq_handlers[interrupt] = handler;
}

void set_exception_handler(uint8_t exception, exception_handler *handler)
{
    exception_handlers[exception] = handler;
}

void handle_interrupt(uint8_t interrupt, uint64_t error_code, cpu_context_t *context)
{
    if (interrupt < 32)
    {
        exception_handler *handler = exception_handlers[interrupt];
        if (handler != NULL)
        {
            handler(context, error_code);
        }
        else
        {
            serial_write("\r\n!!! EXCEPTION: ");
            serial_write(exception_name[interrupt]);
            serial_write(" !!!\r\n");

            screen_write("\n!!! EXCEPTION: ");
            screen_write(exception_name[interrupt]);
            screen_write(" !!!\n");

            while (1)
            {
                __asm__ volatile("hlt");
            }
        }
    }
    else
    {
        irq_handler *handler = irq_handlers[interrupt];
        if (handler != NULL)
        {
            handler();
        }
    }
}
