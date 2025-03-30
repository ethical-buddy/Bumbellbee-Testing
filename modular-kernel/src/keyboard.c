#include "kernel.h"

void kb_init(void)
{
    /* 0xFD = 11111101 : enables only IRQ1 (keyboard) */
    write_port(0x21, 0xFD);
}

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

