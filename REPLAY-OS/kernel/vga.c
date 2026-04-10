#include <vga.h>
#include <io.h>

#define VIDEO_MEMORY 0xb8000
#define WIDTH 80
#define HEIGHT 25

static uint16_t* terminal_buffer = (uint16_t*)VIDEO_MEMORY;
static int row;
static int col;

void vga_clear() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            terminal_buffer[y * WIDTH + x] = (uint16_t)' ' | (uint16_t)0x0f << 8;
        }
    }
}

void vga_init() {
    row = 0;
    col = 0;
    vga_clear();
}

void vga_putc(char c) {
    if (c == '\n') {
        col = 0;
        row++;
    } else {
        terminal_buffer[row * WIDTH + col] = (uint16_t)c | (uint16_t)0x0f << 8;
        col++;
    }

    if (col >= WIDTH) {
        col = 0;
        row++;
    }

    if (row >= HEIGHT) {
        vga_clear();
        row = 0;
    }
}

void vga_print(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        vga_putc(str[i]);
    }
}
