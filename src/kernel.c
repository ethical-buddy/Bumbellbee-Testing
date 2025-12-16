// kernel.c - Kernel entry point and initialization

// Custom type definitions for freestanding environment
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long size_t;
#define bool _Bool
#define true 1
#define false 0
#define NULL ((void*)0)

// I/O PORT OPERATIONS
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Serial port for debugging
#define SERIAL_COM1 0x3F8
void serial_write(const char* s) {
    while (*s) {
        outb(SERIAL_COM1, *s++);
    }
}

// VGA TEXT MODE

#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_YELLOW = 14,
    VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t*) VGA_MEMORY;
    
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_scroll(void) {
    // Move all lines up by one
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t dest = y * VGA_WIDTH + x;
            const size_t src = (y + 1) * VGA_WIDTH + x;
            terminal_buffer[dest] = terminal_buffer[src];
        }
    }
    
    // Clear last line
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
    
    terminal_row = VGA_HEIGHT - 1;
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
        return;
    }
    
    if (c == '\b') {  // Backspace
        if (terminal_column > 0) {
            terminal_column--;
            terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
        }
        return;
    }
    
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_scroll();
        }
    }
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(data[i]);
    }
}

void terminal_writestring(const char* data) {
    size_t len = 0;
    while (data[len]) len++;
    terminal_write(data, len);
}

// KEYBOARD DRIVER

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// US QWERTY scancode to ASCII mapping (scan code set 1)
static const char scancode_to_ascii[] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

static bool shift_pressed = false;

void isr_keyboard_handler(void) {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    // Check if key release (bit 7 set)
    if (scancode & 0x80) {
        scancode &= 0x7F;
        if (scancode == 0x2A || scancode == 0x36) {  // Left/Right shift released
            shift_pressed = false;
        }
        return;
    }
    
    // Check for shift press
    if (scancode == 0x2A || scancode == 0x36) {  // Left/Right shift
        shift_pressed = true;
        return;
    }
    
    // Convert scancode to ASCII
    if (scancode < sizeof(scancode_to_ascii)) {
        char c = scancode_to_ascii[scancode];
        if (c != 0) {
            // Apply shift modifier
            if (shift_pressed && c >= 'a' && c <= 'z') {
                c = c - 'a' + 'A';
            } else if (shift_pressed) {
                // Shift numbers and symbols
                const char shift_map[] = {')', '!', '@', '#', '$', '%', '^', '&', '*', '('};
                if (c >= '0' && c <= '9') {
                    c = shift_map[c - '0'];
                }
            }
            // Output to both VGA display and serial
            terminal_putchar(c);
            outb(SERIAL_COM1, c);
        }
    }
}

// GDT AND IDT SETUP

// External assembly functions
extern void gdt_load(void);
extern void idt_load(void);
extern void idt_set_gate(uint8_t num, uint32_t handler, uint16_t selector, uint8_t flags);
extern void isr_stub_keyboard(void);
extern void isr_stub_timer(void);

void init_gdt(void) {
    gdt_load();
}

void init_idt(void) {
    // Set up keyboard interrupt (IRQ1 = interrupt 33)
    idt_set_gate(33, (uint32_t)isr_stub_keyboard, 0x08, 0x8E);
    // Set up timer interrupt (IRQ0 = interrupt 32)
    idt_set_gate(32, (uint32_t)isr_stub_timer, 0x08, 0x8E);
    idt_load();
}

// PIC (PROGRAMMABLE INTERRUPT CONTROLLER) SETUP

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

void init_pic(void) {
    // ICW1: Initialize
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    // ICW2: Remap IRQs to 32-47
    outb(PIC1_DATA, 0x20);  // Master PIC starts at 32
    outb(PIC2_DATA, 0x28);  // Slave PIC starts at 40
    outb(PIC1_DATA, 0x04);  // Slave on IRQ2
    outb(PIC2_DATA, 0x02);  // Slave identity
    // ICW4: 8086 mode
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    // Unmask all IRQs except cascade
    outb(PIC1_DATA, 0x00);  // Enable all IRQs on master
    outb(PIC2_DATA, 0x00);  // Enable all IRQs on slave
}

// KERNEL MAIN

void kernel_main(uint32_t magic, uint32_t addr) {
    (void)magic;  // Suppress unused warning
    (void)addr;
    
    // Initialize VGA - boot.asm already cleared it
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t*) VGA_MEMORY;
    
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    
    // Debug to serial
    serial_write("[DEBUG] Kernel started\n");
    
    terminal_writestring("\n======================================\n");
    serial_write("\n======================================\n");
    terminal_writestring("  Welcome to Bumbellbee Kernel v0.1\n");
    serial_write("  Welcome to Bumbellbee Kernel v0.1\n");
    terminal_writestring("======================================\n\n");
    serial_write("======================================\n\n");
    
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    terminal_writestring("[*] Initializing GDT...\n");
    serial_write("[*] Initializing GDT...\n");
    init_gdt();
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal_writestring("[OK] GDT loaded\n\n");
    serial_write("[OK] GDT loaded\n\n");
    
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    terminal_writestring("[*] Initializing PIC...\n");
    serial_write("[*] Initializing PIC...\n");
    init_pic();
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal_writestring("[OK] PIC configured\n\n");
    serial_write("[OK] PIC configured\n\n");
    
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    terminal_writestring("[*] Initializing IDT...\n");
    serial_write("[*] Initializing IDT...\n");
    init_idt();
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal_writestring("[OK] IDT loaded\n\n");
    serial_write("[OK] IDT loaded\n\n");
    
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    terminal_writestring("[*] Enabling interrupts...\n");
    serial_write("[*] Enabling interrupts...\n");
    __asm__ __volatile__("sti");
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal_writestring("[OK] Interrupts enabled\n\n");
    serial_write("[OK] Interrupts enabled\n\n");
    
    terminal_setcolor(vga_entry_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK));
    terminal_writestring("System ready! Type to test keyboard... haha MF!s wrote this in 3 hrs\n\n");
    serial_write("System ready! Type to test keyboard...  haha Mf!s wrote this in 3 hrs\n\n");
    terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    terminal_writestring("> ");
    serial_write("> ");
    
    // Infinite loop - interrupts will handle keyboard
    while(1) {
        __asm__ __volatile__("hlt");
    }
}
