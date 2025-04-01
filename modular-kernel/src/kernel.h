#ifndef KERNEL_H
#define KERNEL_H

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
#include <stdint.h>
#include "keyboard_map.h"

/* --- External assembly functions --- */
//extern void keyboard_handler(void);
extern void page_fault_handler(void);
extern char read_port(unsigned short port);
extern void write_port(unsigned short port, unsigned char data);
extern void load_idt(unsigned long *idt_ptr);

/* --- Global video memory variables --- */
extern unsigned int current_loc;
extern char *vidptr;

/* --- IDT entry structure --- */
struct IDT_entry {
    unsigned short int offset_lowerbits;
    unsigned short int selector;
    unsigned char zero;
    unsigned char type_attr;
    unsigned short int offset_higherbits;
};

/* --- Global IDT --- */
extern struct IDT_entry IDT[IDT_SIZE];

/* Function prototypes for each module */
void idt_init(void);
void kb_init(void);
void kprint(const char *str);
void kprint_newline(void);
void putch(unsigned char c);
void clear_screen(void);
void keyboard_handler_main(void);
void page_fault_handler_main(unsigned int error_code);
void init_paging(void);
uint32_t* create_page_directory();
void kprint_hex(unsigned int n);
void *kmalloc(unsigned int size);
void *kmalloc_aligned(uint32_t size);
void kmain(void);

#endif

