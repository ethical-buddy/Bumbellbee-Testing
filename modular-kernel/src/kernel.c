#include "kernel.h"
#include "proc.h"
#include "irq.h"
#include "isr.h"
#include "timer.h"
#include "kb.h"
void pid0_print(){
    kprint("1");
}


void kmain(void)
{
    const char *str = "Bumbellbee with Kernel Features: Basic Paging, IDT/PIC Remapping, Keyboard Input, Bump & Dynamic Heap, Exception Handling, Dynamic Memory Management, and Process Management with Scheduling, ISR, IRQ ,PIC, PIT-timer";    
    clear_screen();
    kprint(str);
    kprint_newline();
    kprint_newline();

    /* Initialize basic paging */
    init_paging();

    /* Initialize the IDT (including the page fault and keyboard handlers) */
    idt_init();
	
    /* install ISR's */
    isr_install();

    /* Install IRQ's */
    irq_install();

    /* Enable PIC */
    timer_install();

    keyboard_install();
    /* Enable keyboard interrupts */
    asm volatile ("sti");
    mm_init();

    /* Demonstrate kmalloc usage */
    char *demo = (char*)kmalloc(32);
    demo[0] = 'H'; demo[1] = 'i'; demo[2] = '\0';
    kprint("Allocated heap memory says: ");
    kprint(demo);
    kprint_newline();

    /* Processes */

    proc_init();
    process_t* p0 = proc_create(pid0_print);
    proc_yield();

    while(1);  /* Halt the kernel loop */
}

