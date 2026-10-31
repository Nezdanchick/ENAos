#pragma once

#include <stdint.h>
#include <stddef.h>

void screen_init(void);
void screen_clear(void);
void screen_putc(char c);
void screen_write(char *string);
