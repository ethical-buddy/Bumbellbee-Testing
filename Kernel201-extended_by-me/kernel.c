#include "keyboard_map.h"

/* --- Definitions for screen output --- */
#define LINES 25
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE (BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES)

/* --- Ports and interrupt constants --- */
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08
#define ENTER_KEY_CODE 0x1C

/* --- Paging and Memory --- */
#define PAGE_DIRECTORY_ENTRIES 1024
#define PAGE_TABLE_ENTRIES 1024

/* --- Kernel Heap --- */
#define KERNEL_HEAP_SIZE (1024 * 1024)  /* 1MB kernel heap */

/* --- External assembly functions --- */
extern void keyboard_handler(void);
extern void page_fault_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt_ptr);

/* --- Global video memory variables --- */
unsigned int current_loc = 0;
char *vidptr = (char*)0xb8000;

/* --- IDT entry structure --- */
struct IDT_entry {
    unsigned short int offset_lowerbits;
    unsigned short int selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short int offset_higherbits;
};

/* --- Global IDT --- */
struct IDT_entry IDT[IDT_SIZE];

/* --- Kernel Heap (bump allocator) --- */
static char kernel_heap[KERNEL_HEAP_SIZE];
static unsigned int heap_offset = 0;

/* --- Function Prototypes --- */
void idt_init(void);
void kb_init(void);
void kprint(const char *str);
void kprint_newline(void);
void clear_screen(void);
void keyboard_handler_main(void);
void page_fault_handler_main(unsigned int error_code);
void init_paging(void);
void kprint_hex(unsigned int n);
void *kmalloc(unsigned int size);

/* --- IDT Initialization (including PIC remapping and new page fault vector) --- */
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

/* --- Keyboard Initialization --- */
void kb_init(void)
{
    /* 0xFD = 11111101 : enables only IRQ1 (keyboard) */
    write_port(0x21, 0xFD);
}

/* --- Basic Printing Functions --- */
void kprint(const char *str)
{
    unsigned int i = 0;
    while (str[i] != '\0') {
        vidptr[current_loc++] = str[i++];
        vidptr[current_loc++] = 0x07;
    }
}

void kprint_newline(void)
{
    unsigned int line_size = BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE;
    current_loc = current_loc + (line_size - (current_loc % line_size));
}

void clear_screen(void)
{
    unsigned int i = 0;
    while (i < SCREENSIZE) {
        vidptr[i++] = ' ';
        vidptr[i++] = 0x07;
    }
}

/* --- Hexadecimal Printing Helper --- */
void kprint_hex(unsigned int n)
{
    char hex_digits[] = "0123456789ABCDEF";
    char buffer[9];
    int i;
    buffer[8] = '\0';
    for (i = 7; i >= 0; i--) {
        buffer[i] = hex_digits[n & 0xF];
        n >>= 4;
    }
    kprint(buffer);
}

/* --- Keyboard Interrupt Handler --- */
void keyboard_handler_main(void)
{
    unsigned char status;
    char keycode;

    /* Signal End Of Interrupt (EOI) to PIC */
    write_port(0x20, 0x20);

    status = read_port(KEYBOARD_STATUS_PORT);
    if (status & 0x01) {
        keycode = read_port(KEYBOARD_DATA_PORT);
        if (keycode < 0)
            return;

        if (keycode == ENTER_KEY_CODE) {
            kprint_newline();
            return;
        }
        vidptr[current_loc++] = keyboard_map[(unsigned char) keycode];
        vidptr[current_loc++] = 0x07;
    }
}

/* --- Page Fault Handler --- 
     This function is called with the error code pushed by the CPU.
*/
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

/* --- Basic Paging Initialization --- */
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

/* --- Simple Kernel Heap Allocator (Bump Allocator) --- */
void *kmalloc(unsigned int size)
{
    void *ptr = (void*)&kernel_heap[heap_offset];
    heap_offset += size;
    if (heap_offset >= KERNEL_HEAP_SIZE) {
        kprint("Out of kernel heap memory!");
        while (1);  /* Halt if out of memory */
    }
    return ptr;
}

/* --- Main Kernel Function --- */
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

