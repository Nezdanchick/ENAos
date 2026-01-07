#pragma once

#pragma once

#include <com.h>
#include <stdint.h>

extern int init_serial(enum Port port);
extern void putserial(char character);
extern void serial_write(char *string);
