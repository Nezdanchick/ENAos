#include <mmu/pmm.h>
#include <mmu/vmm.h>
#include <memory.h>
#include <stdio.h>
#include <panic.h>

#include <string.h>
#include <serial.h>

#define BLOCK_SIZE PAGE_SIZE
#define BITMAP_BLOCKS (BLOCK_SIZE / sizeof(uint32_t) - 1)
#define BITMAP_SIZE sizeof(bitmap_t)
#define RESERVE_BLOCKS 4

#define PRESENT_FLAG 0b0001
#define FREE_FLAG 0b0010
#define BLOCK_NUMBER(flag) (flag >> 16)

// ## BLOCK_NUMBER ###### FLAGS ######
// #     16 BIT     #..............FP#
// ##################fedcba9876543210#
//
// BLOCK_NUMBER the same for one bunch of blocks

typedef struct __attribute__((aligned(BLOCK_SIZE)))
{
    uint16_t index;
    uint16_t block_number;
    uint32_t flags[BITMAP_BLOCKS];
} bitmap_info_t;

typedef struct
{
    bool data[BLOCK_SIZE];
} bitmap_block_t;

typedef struct
{
    bitmap_info_t info;
    bitmap_block_t blocks[BITMAP_BLOCKS];
} bitmap_t;

uint64_t heap = 0x200000;
uint64_t heap_pointer;
bitmap_t *last_bitmap;

uint64_t pmm_get_block_count(uint64_t size);
bitmap_t *pmm_create_bitmap();
bitmap_block_t *pmm_create_block(uint64_t size);

void pmm_init()
{
    printf("INIT PMM...\n");
    heap_pointer = heap;
    last_bitmap = (void *)heap;
    init_map_page(heap, heap);
    recursive_map(heap_pointer, heap_pointer, BITMAP_SIZE);

    if (BLOCK_SIZE < BITMAP_BLOCKS)
        panic("Kernel fault. Bitmap can't contain info about blocks, because block size is too small");
    if (sizeof(bitmap_info_t) != sizeof(bitmap_block_t))
        panic("Kernel fault. Bitmap info size (0x%x) != bitmap block size (0x%x)", sizeof(bitmap_info_t), sizeof(bitmap_block_t));
}

bitmap_t *pmm_create_bitmap()
{
    bitmap_t *bitmap = (void *)heap_pointer;
    memset(bitmap, 0, BITMAP_SIZE); // TODO delete memset 0
    bitmap->info.block_number = 0;
    bitmap->info.index = 0;

    for (uint64_t i = 0; i < BITMAP_BLOCKS; i++)
    {
        bitmap->info.flags[i] |= FREE_FLAG;
    }

    heap_pointer += BITMAP_SIZE;
    recursive_map(heap_pointer, heap_pointer, BITMAP_SIZE);
    printf("create bitmap at 0x%lx\n", bitmap);
    return bitmap;
}
bitmap_block_t *pmm_create_block(uint64_t size)
{
    bitmap_t *bitmap = last_bitmap;
    uint64_t count = pmm_get_block_count(size);
    if (bitmap->info.index + count + RESERVE_BLOCKS > BITMAP_BLOCKS)
    {
        last_bitmap = pmm_create_bitmap();
        return pmm_create_block(size);
    }
    if (count > BITMAP_BLOCKS)
    {
        panic("Kernel fault. Bitmap can't create block of size 0x%lx", size);
    }
    bitmap_block_t *block = &bitmap->blocks[bitmap->info.index];

    for (uint64_t i = 0; i <= count; i++)
    {
        bitmap->info.flags[i + bitmap->info.index] |= PRESENT_FLAG;
    }

    bitmap->info.block_number++;
    bitmap->info.index += count;
    return block;
}

uint64_t pmm_get_block_count(uint64_t size)
{
    uint64_t count = size / BLOCK_SIZE;
    if (size % BLOCK_SIZE != 0)
        count++;
    return count;
}
bitmap_t *pmm_get_bitmap(void *ptr)
{
    uint64_t addr = (uint64_t)ptr - (uint64_t)heap;
    addr = addr / BITMAP_SIZE * BITMAP_SIZE;
    return (bitmap_t *)(addr + heap);
}
uint64_t pmm_get_block_index(void *ptr)
{
    bitmap_t *bitmap = pmm_get_bitmap(ptr);
    uint64_t offset = (uint64_t)ptr - (uint64_t)bitmap;
    return offset / BLOCK_SIZE;
}
void pmm_free_block(void *ptr)
{
    bitmap_t *bitmap = pmm_get_bitmap(ptr);
    uint64_t index = pmm_get_block_index(ptr);
    uint16_t number = BLOCK_NUMBER(bitmap->info.flags[index]);

    if (!(bitmap->info.flags[index] & PRESENT_FLAG))
        panic("Not present at 0x%lx", ptr);

    if (bitmap->info.flags[index] & FREE_FLAG)
        panic("Already free at 0x%lx", ptr);

    for (size_t i = index; i < BITMAP_BLOCKS; i++)
    {
        if (!(bitmap->info.flags[i] & PRESENT_FLAG) || BLOCK_NUMBER(bitmap->info.flags[i]) != number)
            break;

        bitmap->info.flags[i] |= FREE_FLAG;
        memset(&bitmap->blocks[i], 0, BLOCK_SIZE);
    }
}
bitmap_block_t *pmm_search_free_block(uint64_t size)
{
    uint64_t need_blocks = pmm_get_block_count(size);
    bitmap_block_t *block = NULL;

    for (uint64_t ptr = (uint64_t)last_bitmap; ptr > (uint64_t)heap; ptr -= BITMAP_SIZE)
    { // from last to first bitmap
        bitmap_t *bitmap = (void *)ptr;

        uint16_t free_count = 0;
        for (size_t i = 0; i < BITMAP_BLOCKS; i++)
        { // foreach block
            uint32_t flag = bitmap->info.flags[i];
            if (!(flag & PRESENT_FLAG) && !(flag & FREE_FLAG))
            {
                free_count = 0;
                continue;
            }
            if (++free_count == need_blocks)
            {
                block = &bitmap->blocks[i - need_blocks + 1];
            }
        }
    }
    return block;
}
void *kmalloc(uint64_t size)
{
    void *ptr = NULL;
    ptr = pmm_create_block(size);
    if (ptr == NULL)
        ptr = pmm_search_free_block(size);
    return ptr;
}
void free(void *pointer)
{
    // pointer points to block of some bitmap
    if (pointer == NULL) // can't free null pointer
        return;
    pmm_free_block(pointer);
}
void *page_alloc()
{
    return kmalloc(PAGE_SIZE);
}
