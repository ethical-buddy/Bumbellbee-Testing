#include "kernel.h"

static char kernel_heap[KERNEL_HEAP_SIZE];
static unsigned int heap_offset = 0;

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

