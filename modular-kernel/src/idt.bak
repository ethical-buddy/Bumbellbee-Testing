#include "kernel.h"

/* Global IDT declaration */
struct IDT_entry IDT[IDT_SIZE];

void idt_init(void)
{
    unsigned long keyboard_address, page_fault_address;
    unsigned long idt_address;
    unsigned long idt_ptr[2];
    int i;

    /* Populate IDT entry for keyboard interrupt (IRQ1, vector 0x21) */
    keyboard_address = (unsigned long)keyboard_handler;
    IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
    IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
    IDT[0x21].zero = 0;
    IDT[0x21].type_attr = INTERRUPT_GATE;
    IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;

    /* Populate IDT entry for page fault exception (vector 14 / 0x0E) */
    page_fault_address = (unsigned long)page_fault_handler;
    IDT[0x0E].offset_lowerbits = page_fault_address & 0xffff;
    IDT[0x0E].selector = KERNEL_CODE_SEGMENT_OFFSET;
    IDT[0x0E].zero = 0;
    IDT[0x0E].type_attr = INTERRUPT_GATE;
    IDT[0x0E].offset_higherbits = (page_fault_address & 0xffff0000) >> 16;

    /* Clear rest of IDT entries (for safety) */
    for (i = 0; i < IDT_SIZE; i++) {
        if(i != 0x21 && i != 0x0E) {
            IDT[i].offset_lowerbits = 0;
            IDT[i].selector = 0;
            IDT[i].zero = 0;
            IDT[i].type_attr = 0;
            IDT[i].offset_higherbits = 0;
        }
    }

    /* PIC remapping */
    write_port(0x20, 0x11);
    write_port(0xA0, 0x11);
    write_port(0x21, 0x20);
    write_port(0xA1, 0x28);
    write_port(0x21, 0x00);
    write_port(0xA1, 0x00);
    write_port(0x21, 0x01);
    write_port(0xA1, 0x01);
    /* Mask all interrupts initially */
    write_port(0x21, 0xff);
    write_port(0xA1, 0xff);

    /* Fill the IDT descriptor */
    idt_address = (unsigned long)IDT;
    idt_ptr[0] = (sizeof(struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
    idt_ptr[1] = idt_address >> 16;

    load_idt(idt_ptr);
}

