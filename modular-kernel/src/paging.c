#include "kernel.h"

void init_paging(void)
{
    static unsigned int page_directory[PAGE_DIRECTORY_ENTRIES] __attribute__((aligned(4096)));
    static unsigned int first_page_table[PAGE_TABLE_ENTRIES] __attribute__((aligned(4096)));
    int i;

    /* Identity map the first 4MB with present and read/write flags (0x03) */
    for (i = 0; i < PAGE_TABLE_ENTRIES; i++) {
        first_page_table[i] = (i * 0x1000) | 0x03;
    }

    /* Point the first entry of the page directory to our page table */
    page_directory[0] = ((unsigned int)first_page_table) | 0x03;

    /* Mark the rest of the page directory entries as not present */
    for (i = 1; i < PAGE_DIRECTORY_ENTRIES; i++) {
        page_directory[i] = 0;
    }

    /* Load the page directory and enable paging */
    asm volatile (
        "mov %0, %%cr3\n"           /* Load page_directory address into CR3 */
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n"    /* Set the PG bit (bit 31) in CR0 */
        "mov %%eax, %%cr0\n"
        :
        : "r" (page_directory)
        : "eax"
    );
}

