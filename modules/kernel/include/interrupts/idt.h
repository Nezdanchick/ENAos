#pragma once

#include <stdint.h>

extern const uint16_t MASTER_PIC_COMMAND;
extern const uint16_t MASTER_PIC_DATA;
extern const uint16_t SLAVE_PIC_COMMAND;
extern const uint16_t SLAVE_PIC_DATA;

void idt_init(void);
