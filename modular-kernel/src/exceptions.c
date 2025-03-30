#include "kernel.h"

void page_fault_handler_main(unsigned int error_code)
{
    unsigned int faulting_address;
    asm volatile ("mov %%cr2, %0" : "=r" (faulting_address));

    kprint("PAGE FAULT!\nFaulting address: ");
    kprint_hex(faulting_address);
    kprint("\nError code: ");
    kprint_hex(error_code);
    kprint("\nSystem halted.");
    while(1);  /* Halt the kernel */
}

