#include <interrupts/isrs.h>
#include <interrupts/pic.h>
#include <stddef.h>
#include <panic.h>

char *exception_name[] = {
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

irq_handler *irq_handlers[256] = {0};

exception_handler *exception_handlers[32] = {0};

void handle_interrupt(uint8_t interrupt, uint64_t error_code, cpu_context_t *context)
{
    if (interrupt >= 32 && interrupt < 47)
        pic_eoi(interrupt);

    if (interrupt < 32)
    {
        exception_handler *handler = exception_handlers[interrupt];
        if (handler != NULL)
            handler(context, error_code);
        else
            panic("EXCEPTION [%d] %s with error code %d", interrupt, exception_name[interrupt], error_code);
    }
    else
    {
        irq_handler *handler = irq_handlers[interrupt];
        if (handler != NULL)
            handler();
    }
}

void set_irq_handler(uint8_t interrupt, irq_handler *handler)
{
    irq_handlers[interrupt] = handler;
}

void set_exception_handler(uint8_t exception, exception_handler *handler)
{
    exception_handlers[exception] = handler;
}