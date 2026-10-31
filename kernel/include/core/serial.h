#pragma once

#include <stdint.h>
#include "com.h"

int serial_init(enum Port port);
void serial_putc(char c);
void serial_write(char *string);
