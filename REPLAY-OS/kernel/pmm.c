#include <pmm.h>
#include <serial.h>

#define PAGE_SIZE 4096
static uint8_t* bitmap;
static uint64_t total_frames;
static uint64_t bitmap_size;

void pmm_init(uint64_t mem_size) {
    total_frames = mem_size / PAGE_SIZE;
    bitmap_size = total_frames / 8;
    
    // Fixed address for bitmap for now (at 1MB)
    bitmap = (uint8_t*)0x100000;
    
    for (uint64_t i = 0; i < bitmap_size; i++) {
        bitmap[i] = 0;
    }

    // Mark first 2MB as used (kernel + tables + bitmap)
    for (uint64_t i = 0; i < 512; i++) {
        uint64_t frame = i;
        bitmap[frame / 8] |= (1 << (frame % 8));
    }
}

void* pmm_alloc_frame() {
    for (uint64_t i = 0; i < total_frames; i++) {
        if (!(bitmap[i / 8] & (1 << (i % 8)))) {
            bitmap[i / 8] |= (1 << (i % 8));
            return (void*)(i * PAGE_SIZE);
        }
    }
    return NULL;
}

void pmm_free_frame(void* frame) {
    uint64_t i = (uint64_t)frame / PAGE_SIZE;
    bitmap[i / 8] &= ~(1 << (i % 8));
}
