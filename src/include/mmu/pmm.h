#pragma once

#include <stdint.h>

extern void pmm_init();
extern void *malloc(uint64_t size);
extern void free(void *pointer);
extern void *allocate(uint64_t size, uint64_t allign);
extern void memory_block_info(void *pointer);
