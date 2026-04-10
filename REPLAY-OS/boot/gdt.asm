; GDT
gdt_start:
    dq 0x0000000000000000 ; null descriptor
gdt_code_32:
    dw 0xffff    ; limit (bits 0-15)
    dw 0x0000    ; base (bits 0-15)
    db 0x00      ; base (bits 16-23)
    db 10011010b ; 1st flags, type flags
    db 11001111b ; 2nd flags, limit (bits 16-19)
    db 0x00      ; base (bits 24-31)
gdt_data_32:
    dw 0xffff    ; limit (bits 0-15)
    dw 0x0000    ; base (bits 0-15)
    db 0x00      ; base (bits 16-23)
    db 10010010b ; 1st flags, type flags
    db 11001111b ; 2nd flags, limit (bits 16-19)
    db 0x00      ; base (bits 24-31)

; 64-bit GDT descriptors
gdt_code_64:
    dw 0
    dw 0
    db 0
    db 10011010b
    db 00100000b ; L=1 (64-bit)
    db 0
gdt_data_64:
    dw 0
    dw 0
    db 0
    db 10010010b
    db 00000000b
    db 0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG_32 equ gdt_code_32 - gdt_start
DATA_SEG_32 equ gdt_data_32 - gdt_start
CODE_SEG_64 equ gdt_code_64 - gdt_start
DATA_SEG_64 equ gdt_data_64 - gdt_start
