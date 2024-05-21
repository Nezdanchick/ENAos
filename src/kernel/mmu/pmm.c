#include <mmu/pmm.h>
#include <mmu/vmm.h>
#include <memory.h>
#include <stdio.h>
#include <panic.h>

#include <string.h>
#include <serial.h>

#define HEAP_SIZE (1024 * 256)

#define USAGE_FLAG (1 << 0)    // memory block in use
#define PREVIOUS_FLAG (1 << 1) // previous pointer isn't null
#define VALID_FLAG (1 << 7)    // block is valid

// where memory block starts
#define START_BLOCK(block) ((uint64_t)block + sizeof(memory_block_t))
#define NEXT_BLOCK(block) (void *)(BLOCK_START(block) + block->size)
#define CLEAR_BLOCK_INFO(block) memset((void *)block, 0, sizeof(memory_block_t));

typedef struct
{
    uint64_t size;
    uint64_t *previous;
    uint8_t flag;
} memory_block_t;

uint8_t heap[HEAP_SIZE] = {0};
uint64_t heap_start;
uint64_t heap_pointer;
memory_block_t *first_block;
memory_block_t *last_block;

void pmm_init()
{
    heap_start = (uint64_t)heap;

    first_block = (void *)heap_start;
    CLEAR_BLOCK_INFO(first_block);
    first_block->flag = VALID_FLAG | USAGE_FLAG;

    last_block = first_block;
    heap_pointer = START_BLOCK(first_block);
}
// put memory_block_t info and allocale memory
void *malloc(uint64_t size)
{
    if (size == 0)
        return NULL;

    memory_block_t *block;

    if (last_block->flag & VALID_FLAG && !(last_block->flag & USAGE_FLAG)) // if last block is free skip searching
    {
        block = last_block;
        printf("Skip searching\n");
        goto skip_create;
    }

    // search free space
    for (memory_block_t *free_block = (memory_block_t *)last_block->previous;
         free_block->flag & PREVIOUS_FLAG;
         free_block = (memory_block_t *)free_block->previous)
    {
        printf("Searching block at 0x%x\n", free_block);
        if (free_block->flag & USAGE_FLAG)
            continue;
        if (free_block->size >= size) // free_block was founded
        {
            printf("Found free block at 0x%x\n", free_block);
            if (free_block->size > size + sizeof(memory_block_t)) // if can fit other block
            {
                block = free_block;                                                                // block is free_block with smaller size
                memory_block_t *block2 = (void *)(START_BLOCK(free_block) + size);                 // block2 after block
                memory_block_t *next_block = (void *)(START_BLOCK(free_block) + free_block->size); // next_block after free_block
                CLEAR_BLOCK_INFO(block2);
                block2->size = free_block->size - size - sizeof(memory_block_t);
                block2->previous = (uint64_t *)block;
                block2->flag = VALID_FLAG | PREVIOUS_FLAG;
                printf("Setup block2 at 0x%x\n", block2);

                if (!(next_block->flag & VALID_FLAG) || last_block == block) // next_block isn't exist
                    last_block = block2;
                else
                    next_block->previous = (uint64_t *)block2;
            }
            else
            {
                printf("Block now is free_block\n");
                block = free_block;
            }
            goto skip_create;
        }
    }
    // create new block
    {
        if (HEAP_SIZE < (heap_pointer - heap_start) + size + sizeof(memory_block_t))
            panic("pmm: No free space in heap to allocate 0x%x/0x%x", (heap_pointer - heap_start), HEAP_SIZE);

        block = (void *)heap_pointer;
        printf("Match block at 0x%x\n", heap_pointer);

        heap_pointer += sizeof(memory_block_t);
        if (last_block != block) // recursive block falue
            block->previous = (uint64_t *)last_block;

        heap_pointer += size;
        last_block = block;
        printf("Created block at 0x%x\n", START_BLOCK(block));
    }
skip_create:
    block->size = size;
    block->flag = VALID_FLAG | USAGE_FLAG;
    if (block->previous != NULL)
        block->flag |= PREVIOUS_FLAG;
    memset((void *)(START_BLOCK(block)), 0, size);

    return (void *)(START_BLOCK(block));
}
void free(void *pointer)
{
    if (pointer == NULL)
        return;
    memory_block_t *block = pointer - sizeof(memory_block_t);
    printf("Free block at 0x%x of size 0x%x\n", START_BLOCK(block), block->size);

    block->flag &= ~USAGE_FLAG;
}
void *allocate(uint64_t size, uint64_t allign)
{
    if (heap_pointer % allign == 0)
        return malloc(size);
    uint64_t need = heap_pointer % allign;
    uint8_t size_of_info = sizeof(memory_block_t) * 2;

    if (need < size_of_info)
        need = allign - need - size_of_info;
    else
        need -= size_of_info;

    void *free_block = malloc(need);
    void * ptr = malloc(size);
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
