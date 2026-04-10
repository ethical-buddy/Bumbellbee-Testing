[org 0x8000]
[bits 16]

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

    mov ebp, 0x90000 ; Stack at 0x90000
    mov esp, ebp

    ; Enable LM
    ; 1. Check if CPUM (not needed for research OS, assume 64-bit capable)
    ; 2. Setup paging
    call setup_paging
    
    ; 3. Enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; 4. Set LME bit in EFER MSR
    mov ecx, 0xC0000080 ; EFER MSR
    rdmsr
    or eax, 1 << 8      ; LME bit
    wrmsr

    ; 5. Enable Paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    lgdt [gdt_descriptor] ; Load 64-bit GDT
    jmp CODE_SEG_64:init_lm

[bits 32]
setup_paging:
    ; PML4 at 0x1000
    ; PDPT at 0x2000
    ; PDT  at 0x3000
    ; Identity map 2MB

    ; Clear tables
    mov edi, 0x1000
    mov cr3, edi
    xor eax, eax
    mov ecx, 4096
    rep stosd

    mov edi, cr3
    ; PML4 -> PDPT
    mov dword [edi], 0x2003      ; 0x2000 | present | writable
    
    ; PDPT -> PDT
    add edi, 0x1000
    mov dword [edi], 0x3003

    ; PDT -> 2MB Page
    add edi, 0x1000
    mov dword [edi], 0x00000083 ; 0x0 | present | writable | 2MB size

    ret

[bits 64]
init_lm:
    mov ax, DATA_SEG_64
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Jump to Kernel
    extern kernel_main
    call kernel_main

    jmp $
