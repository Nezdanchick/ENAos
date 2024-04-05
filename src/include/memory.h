#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

extern void *memset(void *destination, int value, size_t size);
extern bool memcmp(const void *a, const void *b, uint32_t size);
