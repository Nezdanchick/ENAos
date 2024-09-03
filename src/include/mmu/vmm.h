#pragma once

#include <stdint.h>

#define PAGE_SIZE 4096

extern uint64_t addr_virt_to_phys(uint64_t virt_addr);
extern void map_page(uint64_t phys_addr, uint64_t virt_addr);
extern void recursive_map(uint64_t phys_addr, uint64_t virt_addr, uint64_t size);
