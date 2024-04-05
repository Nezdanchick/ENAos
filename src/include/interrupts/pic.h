#pragma once

#include <stdint.h>

extern void pic_init();
extern void pic_eoi(uint8_t interrupt);
