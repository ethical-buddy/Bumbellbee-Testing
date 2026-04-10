#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>

void pmm_init(uint64_t mem_size);
void* pmm_alloc_frame();
void pmm_free_frame(void* frame);

#endif
