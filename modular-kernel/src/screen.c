#include "kernel.h"

/* Global video memory variables (defined in one module) */
unsigned int current_loc = 0;
char *vidptr = (char*)0xb8000;

void kprint(const char *str)
{
    unsigned int i = 0;
    while (str[i] != '\0') {
        vidptr[current_loc++] = str[i++];
        vidptr[current_loc++] = 0x07;
    }
}

void putch(unsigned char c){
	vidptr[current_loc++] = c;
	vidptr[current_loc++] = 0x07;
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

