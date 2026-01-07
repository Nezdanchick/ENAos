#pragma once

#include <stdint.h>

extern void init_pmm();
extern void *kmalloc(uint64_t size);
extern void free(void *pointer);
extern void *page_alloc();
