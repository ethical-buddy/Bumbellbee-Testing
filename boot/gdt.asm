; gdt.asm - Global Descriptor Table setup
section .data

align 8
gdt_start:
    ; Null descriptor
    dq 0x0

gdt_code:
    ; Code segment descriptor
    ; Base=0, Limit=0xFFFFF, Access=0x9A, Flags=0xC
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0x0          ; Base (bits 0-15)
    db 0x0          ; Base (bits 16-23)
    db 10011010b    ; Access: Present, Ring 0, Code, Executable, Readable
    db 11001111b    ; Flags: Granularity, 32-bit, Limit (bits 16-19)
    db 0x0          ; Base (bits 24-31)

gdt_data:
    ; Data segment descriptor
    ; Base=0, Limit=0xFFFFF, Access=0x92, Flags=0xC
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0x0          ; Base (bits 0-15)
    db 0x0          ; Base (bits 16-23)
    db 10010010b    ; Access: Present, Ring 0, Data, Writable
    db 11001111b    ; Flags: Granularity, 32-bit, Limit (bits 16-19)
    db 0x0          ; Base (bits 24-31)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size
    dd gdt_start                ; Offset

section .text
global gdt_load
global gdt_descriptor

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

gdt_load:
    lgdt [gdt_descriptor]
    
    ; Reload code segment
    jmp CODE_SEG:.reload_cs
.reload_cs:
    ; Reload data segments
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret
