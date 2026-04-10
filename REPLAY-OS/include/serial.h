#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

void init_serial();
void serial_putc(char a);
void serial_print(const char* str);

#endif
