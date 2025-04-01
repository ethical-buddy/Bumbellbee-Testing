#include "kernel.h"

static char kernel_heap[KERNEL_HEAP_SIZE];
static unsigned int heap_offset = 0;
extern uint32_t placement_address;



void *kmalloc(unsigned int size)
{
    void *ptr = (void*)&kernel_heap[heap_offset];
    heap_offset += size;
    if (heap_offset >= KERNEL_HEAP_SIZE) {
        kprint("Out of kernel heap memory!");
        while (1);  /* Halt if out of memory */
    }
    return ptr;
}

void *kmalloc_aligned(uint32_t size)
{
    uint32_t ptr = (uint32_t)&kernel_heap[heap_offset];
    if (ptr & 0xFFFFF000) {
        ptr &= 0xFFFFF000;
        ptr += 0x1000;
    }
    heap_offset += size;
    if (heap_offset >= KERNEL_HEAP_SIZE) {
        kprint("Out of kernel heap memory!");
        while (1);  /* Halt if out of memory */
    }
    return (void*)ptr;

}



