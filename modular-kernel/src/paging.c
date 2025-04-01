#include "kernel.h"
static unsigned int kernel_page_directory[PAGE_DIRECTORY_ENTRIES] __attribute__((aligned(4096)));
static unsigned int first_page_table[PAGE_TABLE_ENTRIES] __attribute__((aligned(4096)));

void init_paging(void)
{
    int i;

    /* Identity map the first 4MB with present and read/write flags (0x03) */
    for (i = 0; i < PAGE_TABLE_ENTRIES; i++) {
        first_page_table[i] = (i * 0x1000) | 0x03;
    }

    /* Point the first entry of the page directory to our page table */
    kernel_page_directory[0] = ((unsigned int)first_page_table) | 0x03;

    /* Mark the rest of the page directory entries as not present */
    for (i = 1; i < PAGE_DIRECTORY_ENTRIES; i++) {
        kernel_page_directory[i] = 0;
    }

    /* Load the page directory and enable paging */
    asm volatile (
        "mov %0, %%cr3\n"           /* Load page_directory address into CR3 */
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n"    /* Set the PG bit (bit 31) in CR0 */
        "mov %%eax, %%cr0\n"
        :
        : "r" (kernel_page_directory)
        : "eax"
    );
}


// Allocate a new page directory for a process.
uint32_t* create_page_directory() {
    // Allocate memory for 1024 entries (page directory entries)
    uint32_t* page_directory = (uint32_t*)kmalloc_aligned(sizeof(uint32_t) * 1024);

    // Initialize all entries to 0
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0;
    }

    // Copy kernel mappings. For instance, if the kernel is mapped in the higher half:
    // Assume kernel mappings are from entry 768 to 1023.
    for (int i = 768; i < 1024; i++) {
        page_directory[i] = kernel_page_directory[i];
    }

    // Set up user space entries as needed (initially empty or with default mappings).
    // This is where you would later map user code, data, etc.
    
    return page_directory;
}
