#pragma once

#include <cpu_context.h>

typedef cpu_context_t* exception_handler(cpu_context_t* context);
typedef void irq_handler(void);

extern void set_irq_handler(uint8_t interrupt, irq_handler* handler);
extern void set_exception_handler(uint8_t exception, exception_handler* handler);
