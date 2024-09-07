#include <mmu/pmm.h>
#include <mmu/vmm.h>
#include <memory.h>
#include <stdio.h>

#include <string.h>
#include <serial.h>

#define USAGE_FLAG (1 << 0)    // memory block in use
#define PREVIOUS_FLAG (1 << 1) // previous pointer isn't null
#define VALID_FLAG (1 << 7)    // block is valid

// where memory block starts
#define START_BLOCK(block) ((uint64_t)block + sizeof(memory_block_t))
#define NEXT_BLOCK(block) (void *)(START_BLOCK(block) + block->size)
#define CLEAR_BLOCK_INFO(block) memset((void *)block, 0, sizeof(memory_block_t));

typedef struct
{
    uint64_t size;
    uint64_t *previous;
    uint8_t flag;
} memory_block_t;

uint64_t *init_heap = (void *)0x100000;
uint64_t *heap = (void *)0x200000;
uint64_t heap_pointer;
memory_block_t *first_block;
memory_block_t *last_block;

void pmm_init()
{
    // pre init before recursive map
    first_block = (void *)init_heap;
    CLEAR_BLOCK_INFO(first_block);
    first_block->flag = VALID_FLAG | USAGE_FLAG;

    last_block = first_block;
    heap_pointer = START_BLOCK(first_block);

    recursive_map((uint64_t)heap, (uint64_t)heap, PAGE_SIZE);

    // init
    first_block = (void *)heap;
    CLEAR_BLOCK_INFO(first_block);
    first_block->flag = VALID_FLAG | USAGE_FLAG;

    last_block = first_block;
    heap_pointer = START_BLOCK(first_block);
}
// put memory_block_t info and clear memory chunk
void *kmalloc(uint64_t size)
{
    if (size == 0)
        return NULL;

    memory_block_t *block;

    if (last_block->flag & VALID_FLAG && !(last_block->flag & USAGE_FLAG)) // if last block is free skip searching
    {
        block = last_block;
        goto skip_create;
    }

    // search free space
    for (memory_block_t *free_block = (memory_block_t *)last_block->previous;
         free_block->flag & PREVIOUS_FLAG;
         free_block = (memory_block_t *)free_block->previous)
    {
        if (free_block->flag & USAGE_FLAG)
            continue;
        if (free_block->size >= size) // free_block was founded
        {
            if (free_block->size == size)
            {
                block = free_block;
            }
            else if (free_block->size > size + sizeof(memory_block_t)) // if can fit other block
            {
                block = free_block;                                                // block is free_block with smaller size
                memory_block_t *block2 = (void *)(START_BLOCK(free_block) + size); // block2 after block
                memory_block_t *next_block = NEXT_BLOCK(free_block);               // next_block after free_block
                CLEAR_BLOCK_INFO(block2);
                block2->size = free_block->size - size - sizeof(memory_block_t);
                block2->previous = (uint64_t *)block;
                block2->flag = VALID_FLAG | PREVIOUS_FLAG;

                if (!(next_block->flag & VALID_FLAG) || last_block == block) // next_block isn't exist
                    last_block = block2;
                else
                    next_block->previous = (uint64_t *)block2;
            }
            else
                continue;
            goto skip_create;
        }
    }
    // create new block
    {
        block = (void *)heap_pointer;
        recursive_map(heap_pointer, heap_pointer, size + PAGE_SIZE);

        heap_pointer += sizeof(memory_block_t);
        if (last_block != block) // recursive block falue
            block->previous = (uint64_t *)last_block;

        heap_pointer += size;
        last_block = block;
    }
skip_create:
    block->size = size;
    block->flag = VALID_FLAG | USAGE_FLAG;
    if (block->previous != NULL)
        block->flag |= PREVIOUS_FLAG;
    void *pointer = (void *)(START_BLOCK(block));
    memset(pointer, 0, size);

    return pointer;
}
void free(void *pointer)
{
    if (pointer == NULL)
        return;
    memory_block_t *block = pointer - sizeof(memory_block_t);

    block->flag &= ~USAGE_FLAG;

    memory_block_t *prev = (memory_block_t *)block->previous;

    if (!(prev->flag & USAGE_FLAG)) // if previos block is free
    {
        if (last_block == block)
        {
            last_block = prev;
        }
        else // in the middle
        {
            memory_block_t *next = NEXT_BLOCK(block);
            next->previous = (uint64_t *)prev;
        }
        CLEAR_BLOCK_INFO(block);
    }
}
void *page_alloc()
{
    if (heap_pointer % PAGE_SIZE == sizeof(memory_block_t))
        return kmalloc(PAGE_SIZE);

    uint64_t need = PAGE_SIZE - (heap_pointer % PAGE_SIZE) - sizeof(memory_block_t) * 2;
    void *free_block = kmalloc(need); // .............| <- allign ......
    void *ptr = kmalloc(PAGE_SIZE);   // | FREE BLOCK | PAGE BLOCK | ...
    free(free_block);
    return ptr;
}
void memory_block_info(void *pointer)
{
    if (pointer == NULL)
        return;
    memory_block_t *block = pointer - sizeof(memory_block_t);

    printf("Block at 0x%x\n"
           "Size: %d\n"
           "Flag: %d\n"
           "Previous block at 0x%x\n",
           START_BLOCK(block), block->size, block->flag, block->previous);
}
