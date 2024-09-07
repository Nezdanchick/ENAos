#include <mmu/vmm.h>
#include <mmu/pmm.h>
#include <stdio.h>
#include <panic.h>

#define HEAP_SIZE 8

#define PAGE_PRESENT (1 << 0)
#define PAGE_WRITE (1 << 1)
#define PAGE_USER (1 << 2)

typedef uint64_t pml4e_t;
typedef uint64_t pdpte_t;
typedef uint64_t pde_t;
typedef uint64_t pte_t;

typedef uint8_t *page_t;

extern pml4e_t page_table_l4[512];
extern pdpte_t page_table_l3[512];
extern pde_t page_table_l2[512];
extern pte_t page_table_l1[512];

uint64_t addr_virt_to_phys(uint64_t virt_addr)
{
    uint16_t pml4_idx = (virt_addr >> 39) & 0x1FF;
    uint16_t pdpt_idx = (virt_addr >> 30) & 0x1FF;
    uint16_t pd_idx = (virt_addr >> 21) & 0x1FF;
    uint16_t pt_idx = (virt_addr >> 12) & 0x1FF;
    uint32_t phys_addr = virt_addr & 0xfff;

    pml4e_t *pml4 = (pml4e_t *)page_table_l4;
    pdpte_t *pdpt = (pdpte_t *)(pml4[pml4_idx] & ~0xFFF);
    pde_t *pd = (pde_t *)(pdpt[pdpt_idx] & ~0xFFF);
    pte_t *pt = (pte_t *)(pd[pd_idx] & ~0xFFF);
    return phys_addr + pt[pt_idx];
}
void map_page(uint64_t phys_addr, uint64_t virt_addr)
{
    uint16_t pml4_idx = (virt_addr >> 39) & 0x1FF;
    uint16_t pdpt_idx = (virt_addr >> 30) & 0x1FF;
    uint16_t pd_idx = (virt_addr >> 21) & 0x1FF;
    uint16_t pt_idx = (virt_addr >> 12) & 0x1FF;

    pml4e_t *pml4 = (uint64_t *)page_table_l4;

    if (!(pml4[pml4_idx] & PAGE_PRESENT))
    {
        uint64_t *pdpt = page_alloc();
        pml4[pml4_idx] = (uint64_t)pdpt | PAGE_PRESENT | PAGE_WRITE;
    }

    pdpte_t *pdpt = (pdpte_t *)(pml4[pml4_idx] & ~0xFFF);

    if (!(pdpt[pdpt_idx] & PAGE_PRESENT))
    {
        uint64_t *pd = page_alloc();
        pdpt[pdpt_idx] = (uint64_t)pd | PAGE_PRESENT | PAGE_WRITE;
    }

    pde_t *pd = (pde_t *)(pdpt[pdpt_idx] & ~0xFFF);

    if (!(pd[pd_idx] & PAGE_PRESENT))
    {
        uint64_t *pt = page_alloc();
        pd[pd_idx] = (uint64_t)pt | PAGE_PRESENT | PAGE_WRITE;
    }

    pte_t *pt = (pte_t *)(pd[pd_idx] & ~0xFFF);

    pt[pt_idx] = (phys_addr & ~0xFFF) | PAGE_PRESENT | PAGE_WRITE;

    asm volatile("invlpg (%0)" : : "r"(virt_addr) : "memory");
}
void recursive_map(uint64_t phys_addr, uint64_t virt_addr, uint64_t size)
{
    phys_addr &= ~0xFFF;
    virt_addr &= ~0xFFF;
    for (uint64_t i = 0; i <= size; i += PAGE_SIZE)
        map_page(phys_addr + i, virt_addr + i);
}
