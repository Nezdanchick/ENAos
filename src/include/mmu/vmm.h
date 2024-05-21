#pragma once

#include <stdint.h>

extern uint64_t addr_virt_to_phys(uint64_t virt_addr);
extern void *allocate_page();
extern void map_page(uint64_t phys_addr, uint64_t virt_addr);
extern void recursive_map(uint64_t phys_addr, uint64_t virt_addr, uint64_t size);
