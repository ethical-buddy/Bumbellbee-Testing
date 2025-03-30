#include "kernel.h"

void kmain(void)
{
    const char *str = "My first kernel with keyboard, paging, page fault handling and basic heap support";
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

    /* Demonstrate kmalloc usage */
    char *demo = (char*)kmalloc(32);
    demo[0] = 'H'; demo[1] = 'i'; demo[2] = '\0';
    kprint("Allocated heap memory says: ");
    kprint(demo);
    kprint_newline();

    while(1);  /* Halt the kernel loop */
}

