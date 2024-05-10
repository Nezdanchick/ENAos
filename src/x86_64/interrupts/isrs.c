#include <interrupts.h>
#include <stddef.h>
#include <panic.h>

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
            panic("EXCEPTION %d ERROR CODE %d", interrupt, error_code);
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