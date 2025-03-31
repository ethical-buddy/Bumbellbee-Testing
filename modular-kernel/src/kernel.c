#include "kernel.h"
#include "proc.h"

void pid0_print(){
    kprint("1");
}

void pid1_print(){
    // asm ("movl $20, %edx");  // Moves 20 into the EAX register
    kprint("2");
}

void kmain(void)
{
    const char *str = "Bumbellbee with Kernel Features: Basic Paging, IDT/PIC Remapping, Keyboard Input, Bump & Dynamic Heap, Exception Handling, Dynamic Memory Management, and Process Management with Scheduling";
    clear_screen();
    kprint(str);
    kprint_newline();
    kprint_newline();

    /* Initialize basic paging */
    init_paging();

    /* Initialize the IDT (including the page fault and keyboard handlers) */
    idt_init();

    /* Enable keyboard interrupts */
    kb_init();

    mm_init();

    /* Demonstrate kmalloc usage */
    char *demo = (char*)kmalloc(32);
    demo[0] = 'H'; demo[1] = 'i'; demo[2] = '\0';
    kprint("Allocated heap memory says: ");
    kprint(demo);
    kprint_newline();

    /* Processes */

    proc_init();
    process_t* pid0 = proc_create(pid0_print);
    process_t* pid1 = proc_create(pid1_print);
    proc_schedule();
    proc_yield();



    while(1);  /* Halt the kernel loop */
}

