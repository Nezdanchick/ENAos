#pragma once

#include <stdint.h>
#include "cpu_context.h"

typedef void exception_handler(cpu_context_t *context, uint64_t error_code);
typedef void irq_handler(void);

void set_irq_handler(uint8_t interrupt, irq_handler *handler);
void set_exception_handler(uint8_t exception, exception_handler *handler);
void handle_interrupt(uint8_t interrupt, uint64_t error_code, cpu_context_t *context);
