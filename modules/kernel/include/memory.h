#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <mmu/pmm.h>
#include <mmu/vmm.h>

extern void *memset(void *destination, int value, size_t size);
extern bool memcmp(const void *a, const void *b, uint32_t size);
extern void *memcpy(void *destination, const void *source, size_t count);
extern void *memmove(void *dest, const void *src, size_t n);
