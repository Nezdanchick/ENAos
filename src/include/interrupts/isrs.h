#pragma once

#include <cpu_context.h>

typedef void exception_handler(cpu_context_t* context, uint64_t error_code);
typedef void irq_handler(void);

extern void set_irq_handler(uint8_t interrupt, irq_handler* handler);
extern void set_exception_handler(uint8_t exception, exception_handler* handler);
