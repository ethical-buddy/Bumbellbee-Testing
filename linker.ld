/* linker.ld - Linker script for the kernel */

ENTRY(_start)

PHDRS
{
    text PT_LOAD FLAGS(5);  /* 5 = RX (read + execute) */
    data PT_LOAD FLAGS(6);  /* 6 = RW (read + write) */
    note PT_NOTE FLAGS(4);  /* 4 = R */
}

SECTIONS
{
    /* Kernel starts at 1MB */
    . = 1M;

    /* PVH ELF note for QEMU 10.x */
    .note.Xen : ALIGN(4)
    {
        *(.note.Xen)
    } :text :note

    /* Multiboot header must be early */
    .multiboot :
    {
        *(.multiboot)
    } :text

    /* Read-only code and data */
    .text ALIGN(4K) : AT(ADDR(.text))
    {
        *(.text)
    } :text

    .rodata ALIGN(4K) : AT(ADDR(.rodata))
    {
        *(.rodata)
    } :text

    /* Read-write data */
    .data ALIGN(4K) : AT(ADDR(.data))
    {
        *(.data)
    } :data

    /* BSS (uninitialized data) */
    .bss ALIGN(4K) : AT(ADDR(.bss))
    {
        *(COMMON)
        *(.bss)
    } :data

    /* Stack */
    .stack ALIGN(4K) : AT(ADDR(.stack))
    {
        *(.stack)
    } :data
}
