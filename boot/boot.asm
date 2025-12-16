; boot.asm - Multiboot2 bootloader entry point
; This file sets up the initial boot environment for the kernel

section .note.GNU-stack noalloc noexec nowrite progbits

; PVH ELF note for QEMU 10.x compatibility
section .note.Xen
align 4
    dd 0x4       ; namesz (length of "Xen" + null)
    dd 0x4       ; descsz (length of desc)
    dd 0x12      ; type (XEN_ELFNOTE_PHYS32_ENTRY)
    db "Xen", 0  ; name
align 4
    dd 0x101000  ; desc (physical entry point address)

section .multiboot
align 8
multiboot_start:
    dd 0xe85250d6                ; magic number (multiboot2)
    dd 0                         ; architecture 0 (protected mode i386)
    dd multiboot_end - multiboot_start ; header length
    ; checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (multiboot_end - multiboot_start))

    ; end tag
    dw 0
    dw 0
    dd 8
multiboot_end:

section .bss
align 16
stack_bottom:
    resb 16384                   ; 16 KB stack
stack_top:

section .text
global _start
extern kernel_main

_start:
    ; Set up the stack
    mov esp, stack_top
    mov ebp, esp

    ; Ensure interrupts are disabled
    cli
    
    ; Early serial debug - write 'B' to COM1 (0x3F8)
    mov dx, 0x3F8
    mov al, 'B'
    out dx, al
    mov al, 'O'
    out dx, al
    mov al, 'O'
    out dx, al
    mov al, 'T'
    out dx, al
    mov al, 10      ; newline
    out dx, al
    
    ; Clear VGA memory - initialize to spaces
    mov edi, 0xB8000
    mov ecx, 2000           ; 80*25 characters
    mov ax, 0x0720          ; Space with light gray on black
    rep stosw

    ; Push multiboot information for kernel
    push ebx                     ; Multiboot information structure
    push eax                     ; Multiboot magic number

    ; Call the kernel main function
    call kernel_main

    ; Hang if kernel_main returns
.hang:
    hlt
    jmp .hang
