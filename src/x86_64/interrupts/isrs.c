#include <interrupts.h>
#include <screen.h>
#include <stddef.h>

static irq_handler *irq_handlers[256] = {0};

static exception_handler *exception_handlers[32] = {0};

static cpu_context_t *handle_irq(uint8_t interrupt, cpu_context_t *context)
{
    if (interrupt >= 32 && interrupt < 47)
        pic_eoi(interrupt);

    irq_handler *handler = irq_handlers[interrupt];

    if (handler != NULL)
        handler();

    return context;
}

cpu_context_t *handle_soft_irq(uint8_t interrupt, cpu_context_t *context)
{
    return handle_irq(interrupt, context);
}

cpu_context_t *handle_hard_irq(uint8_t interrupt, cpu_context_t *context)
{
    return handle_irq(interrupt, context);
}

cpu_context_t *handle_exception(uint8_t exception, uint64_t error_code, cpu_context_t *context)
{
    exception_handler *handler = exception_handlers[exception];

    if (handler != NULL)
        context = handler(context);

    return context;
}

void set_irq_handler(uint8_t interrupt, irq_handler *handler)
{
    irq_handlers[interrupt] = handler;
}

void set_exception_handler(uint8_t exception, exception_handler *handler)
{
    exception_handlers[exception] = handler;
}