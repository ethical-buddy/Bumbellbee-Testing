[bits 16]
section .entry
global _start
_start:
    ; Switch to PM
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG_32:init_pm

%include "boot/gdt.asm"

[bits 32]
init_pm:
    mov ax, DATA_SEG_32
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000
    mov esp, ebp

    call setup_paging
    
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    lgdt [gdt_descriptor]
    jmp CODE_SEG_64:init_lm

[bits 32]
setup_paging:
    mov edi, 0x1000
    mov cr3, edi
    xor eax, eax
    mov ecx, 4096
    rep stosd

    mov edi, cr3
    mov dword [edi], 0x2003
    add edi, 0x1000
    mov dword [edi], 0x3003
    add edi, 0x1000
    
    ; Map 128MB (64 entries of 2MB each)
    mov eax, 0x00000083 ; Present, Writable, PS (2MB)
    mov ecx, 64
.pd_loop:
    mov [edi], eax
    add edi, 8
    add eax, 0x200000
    loop .pd_loop

    ret

[bits 64]
init_lm:
    mov ax, DATA_SEG_64
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    extern kernel_main
    call kernel_main

    jmp $
