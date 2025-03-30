#include "mm.h"
#include "kernel.h"  // for kprint

#define MM_HEAP_SIZE (512 * 1024)  // 512KB dedicated for dynamic allocations

static char mm_heap[MM_HEAP_SIZE];

typedef struct block_header {
    size_t size;
    struct block_header *next;
} block_header_t;

static block_header_t *free_list = 0;

void mm_init(void)
{
    free_list = (block_header_t*) mm_heap;
    free_list->size = MM_HEAP_SIZE - sizeof(block_header_t);
    free_list->next = 0;
}

void *mm_alloc(size_t size)
{
    // Align size to 8 bytes
    size = (size + 7) & ~7;
    block_header_t *prev = 0, *curr = free_list;
    while (curr) {
        if (curr->size >= size) {
            if (curr->size >= size + sizeof(block_header_t) + 8) { // split block
                block_header_t *new_block = (block_header_t*)((char*)curr + sizeof(block_header_t) + size);
                new_block->size = curr->size - size - sizeof(block_header_t);
                new_block->next = curr->next;
                curr->size = size;
                if (prev)
                    prev->next = new_block;
                else
                    free_list = new_block;
            } else {
                if (prev)
                    prev->next = curr->next;
                else
                    free_list = curr->next;
            }
            return (void*)((char*)curr + sizeof(block_header_t));
        }
        prev = curr;
        curr = curr->next;
    }
    return 0; // Out of memory
}

void mm_free(void *ptr)
{
    if (!ptr) return;
    block_header_t *block = (block_header_t*)((char*)ptr - sizeof(block_header_t));
    block->next = free_list;
    free_list = block;
}

int map_page(void *virt_addr, void *phys_addr, uint32_t flags)
{
    // Minimal stub. A real implementation would update page tables.
    return 0;
}

int unmap_page(void *virt_addr)
{
    // Minimal stub.
    return 0;
}

