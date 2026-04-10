#include <ide.h>
#include <io.h>

#define IDE_PORT 0x1F0

void ide_wait_bsy() {
    while (inb(IDE_PORT + 7) & 0x80);
}

void ide_wait_drq() {
    while (!(inb(IDE_PORT + 7) & 0x08));
}

void ide_read_sector(uint32_t lba, uint8_t* buffer) {
    ide_wait_bsy();
    outb(IDE_PORT + 2, 1);
    outb(IDE_PORT + 3, (uint8_t)lba);
    outb(IDE_PORT + 4, (uint8_t)(lba >> 8));
    outb(IDE_PORT + 5, (uint8_t)(lba >> 16));
    outb(IDE_PORT + 6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(IDE_PORT + 7, 0x20);

    ide_wait_bsy();
    ide_wait_drq();

    uint16_t* ptr = (uint16_t*)buffer;
    for (int i = 0; i < 256; i++) {
        __asm__ volatile ("inw %%dx, %%ax" : "=a"(ptr[i]) : "d"(IDE_PORT));
    }
}

void ide_write_sector(uint32_t lba, uint8_t* buffer) {
    ide_wait_bsy();
    outb(IDE_PORT + 2, 1);
    outb(IDE_PORT + 3, (uint8_t)lba);
    outb(IDE_PORT + 4, (uint8_t)(lba >> 8));
    outb(IDE_PORT + 5, (uint8_t)(lba >> 16));
    outb(IDE_PORT + 6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(IDE_PORT + 7, 0x30);

    ide_wait_bsy();
    ide_wait_drq();

    uint16_t* ptr = (uint16_t*)buffer;
    for (int i = 0; i < 256; i++) {
        __asm__ volatile ("outw %%ax, %%dx" : : "a"(ptr[i]), "d"(IDE_PORT));
    }
}
