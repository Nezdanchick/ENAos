#pragma once

#include <stdint.h>

extern void pmm_init();
extern void *kmalloc(uint64_t size);
extern void free(void *pointer);
extern void *page_alloc();
