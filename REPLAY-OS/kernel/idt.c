#include <idt.h>
#include <serial.h>
#include <vga.h>
#include <io.h>

static idt_entry_t idt[256];
static idtr_t idtr;

extern void idt_load(idtr_t*);

// ISR stubs from ASM
extern void isr_stub_0();
extern void isr_stub_1();
extern void isr_stub_2();
extern void isr_stub_3();
extern void isr_stub_4();
extern void isr_stub_5();
extern void isr_stub_6();
extern void isr_stub_7();
extern void isr_stub_8();
extern void isr_stub_9();
extern void isr_stub_10();
extern void isr_stub_11();
extern void isr_stub_12();
extern void isr_stub_13();
extern void isr_stub_14();
extern void isr_stub_15();
extern void isr_stub_16();
extern void isr_stub_17();
extern void isr_stub_18();
extern void isr_stub_19();
extern void isr_stub_20();
extern void isr_stub_21();
extern void isr_stub_22();
extern void isr_stub_23();
extern void isr_stub_24();
extern void isr_stub_25();
extern void isr_stub_26();
extern void isr_stub_27();
extern void isr_stub_28();
extern void isr_stub_29();
extern void isr_stub_30();
extern void isr_stub_31();

extern void irq_stub_0();
extern void irq_stub_1();
extern void irq_stub_2();
extern void irq_stub_3();
extern void irq_stub_4();
extern void irq_stub_5();
extern void irq_stub_6();
extern void irq_stub_7();
extern void irq_stub_8();
extern void irq_stub_9();
extern void irq_stub_10();
extern void irq_stub_11();
extern void irq_stub_12();
extern void irq_stub_13();
extern void irq_stub_14();
extern void irq_stub_15();

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_low    = (uint64_t)isr & 0xFFFF;
    descriptor->kernel_cs  = 0x18; // 64-bit code segment (CODE_SEG_64 in GDT)
    descriptor->ist        = 0;
    descriptor->attributes = flags;
    descriptor->isr_mid    = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high   = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved   = 0;
}

void idt_init() {
    idtr.base = (uint64_t)&idt;
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * 256 - 1;

    // Exceptions
    idt_set_descriptor(0, isr_stub_0, 0x8E);
    idt_set_descriptor(1, isr_stub_1, 0x8E);
    idt_set_descriptor(2, isr_stub_2, 0x8E);
    idt_set_descriptor(3, isr_stub_3, 0x8E);
    idt_set_descriptor(4, isr_stub_4, 0x8E);
    idt_set_descriptor(5, isr_stub_5, 0x8E);
    idt_set_descriptor(6, isr_stub_6, 0x8E);
    idt_set_descriptor(7, isr_stub_7, 0x8E);
    idt_set_descriptor(8, isr_stub_8, 0x8E);
    idt_set_descriptor(9, isr_stub_9, 0x8E);
    idt_set_descriptor(10, isr_stub_10, 0x8E);
    idt_set_descriptor(11, isr_stub_11, 0x8E);
    idt_set_descriptor(12, isr_stub_12, 0x8E);
    idt_set_descriptor(13, isr_stub_13, 0x8E);
    idt_set_descriptor(14, isr_stub_14, 0x8E);
    idt_set_descriptor(15, isr_stub_15, 0x8E);
    idt_set_descriptor(16, isr_stub_16, 0x8E);
    idt_set_descriptor(17, isr_stub_17, 0x8E);
    idt_set_descriptor(18, isr_stub_18, 0x8E);
    idt_set_descriptor(19, isr_stub_19, 0x8E);
    idt_set_descriptor(20, isr_stub_20, 0x8E);
    idt_set_descriptor(21, isr_stub_21, 0x8E);
    idt_set_descriptor(22, isr_stub_22, 0x8E);
    idt_set_descriptor(23, isr_stub_23, 0x8E);
    idt_set_descriptor(24, isr_stub_24, 0x8E);
    idt_set_descriptor(25, isr_stub_25, 0x8E);
    idt_set_descriptor(26, isr_stub_26, 0x8E);
    idt_set_descriptor(27, isr_stub_27, 0x8E);
    idt_set_descriptor(28, isr_stub_28, 0x8E);
    idt_set_descriptor(29, isr_stub_29, 0x8E);
    idt_set_descriptor(30, isr_stub_30, 0x8E);
    idt_set_descriptor(31, isr_stub_31, 0x8E);

    // IRQs
    idt_set_descriptor(32, irq_stub_0, 0x8E);
    idt_set_descriptor(33, irq_stub_1, 0x8E);
    idt_set_descriptor(34, irq_stub_2, 0x8E);
    idt_set_descriptor(35, irq_stub_3, 0x8E);
    idt_set_descriptor(36, irq_stub_4, 0x8E);
    idt_set_descriptor(37, irq_stub_5, 0x8E);
    idt_set_descriptor(38, irq_stub_6, 0x8E);
    idt_set_descriptor(39, irq_stub_7, 0x8E);
    idt_set_descriptor(40, irq_stub_8, 0x8E);
    idt_set_descriptor(41, irq_stub_9, 0x8E);
    idt_set_descriptor(42, irq_stub_10, 0x8E);
    idt_set_descriptor(43, irq_stub_11, 0x8E);
    idt_set_descriptor(44, irq_stub_12, 0x8E);
    idt_set_descriptor(45, irq_stub_13, 0x8E);
    idt_set_descriptor(46, irq_stub_14, 0x8E);
    idt_set_descriptor(47, irq_stub_15, 0x8E);

    // Remap PIC
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20); // Master offset 32
    outb(0xA1, 0x28); // Slave offset 40
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    idt_load(&idtr);
    __asm__ volatile ("sti");
}

void isr_handler(interrupt_frame_t* frame) {
    serial_print("Exception occurred! Vector: ");
    // Convert int_no to string or just print a generic message for now
    char buf[32];
    vga_print("CPU Exception: ");
    
    if (frame->int_no == 14) { // Page Fault
        uint64_t cr2;
        __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
        serial_print("Page Fault at: ");
        // We'll just print a message since we don't have a full printf yet
        vga_print("Page Fault\n");
    } else {
        vga_print("Error\n");
    }
    
    while(1);
}

extern void timer_callback();
extern void keyboard_callback();

void irq_handler(interrupt_frame_t* frame) {
    if (frame->int_no == 32) {
        timer_callback();
    } else if (frame->int_no == 33) {
        keyboard_callback();
    }

    if (frame->int_no >= 40) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);
}
