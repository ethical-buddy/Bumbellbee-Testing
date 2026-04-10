ENTRY(_start)
OUTPUT_FORMAT(binary)

SECTIONS
{
    . = 0x8000;

    .text :
    {
        *(.entry)
        *(.text)
    }

    .data :
    {
        *(.data)
    }

    .rodata :
    {
        *(.rodata)
    }

    .bss :
    {
        *(.bss)
        *(COMMON)
    }
}
