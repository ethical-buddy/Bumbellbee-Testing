#ifndef MM_H
#define MM_H

#include <stddef.h>
#include <stdint.h>

#define PAGE_PRESENT 0x1
#define PAGE_RW      0x2
#define PAGE_USER    0x4

void mm_init(void);
void *mm_alloc(size_t size);
void mm_free(void *ptr);

// Dynamic page mapping (stub implementations for now)
int map_page(void *virt_addr, void *phys_addr, uint32_t flags);
int unmap_page(void *virt_addr);

#endif

