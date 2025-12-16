; bootloader.asm - Simple 512-byte bootloader for legacy BIOS boot
; This will be sector 0 of a floppy/disk image

BITS 16
ORG 0x7C00

start:
    ; Clear interrupts
    cli
    
    ; Set up segments
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    
    ; Load kernel from disk
    ; We'll load it to 0x1000:0x0000 (physical 0x10000)
    mov bx, 0x1000
    mov es, bx
    xor bx, bx
    
    mov ah, 0x02        ; BIOS read sector function
    mov al, 20          ; Read 20 sectors (10KB kernel)
    mov ch, 0           ; Cylinder 0
    mov cl, 2           ; Start from sector 2
    mov dh, 0           ; Head 0
    mov dl, 0x80        ; First hard drive
    int 0x13
    jc load_failed
    
    ; Enter protected mode
    lgdt [gdt_descriptor]
    
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    jmp 0x08:protected_mode

load_failed:
    mov si, error_msg
    call print_string
    hlt

print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_string
.done:
    ret

error_msg: db "Load failed!", 0

; GDT for protected mode
align 8
gdt_start:
    dq 0x0                          ; Null descriptor

gdt_code:
    dw 0xFFFF                       ; Limit
    dw 0x0                          ; Base (low)
    db 0x0                          ; Base (mid)
    db 10011010b                    ; Access
    db 11001111b                    ; Granularity + Limit (high)
    db 0x0                          ; Base (high)

gdt_data:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

BITS 32
protected_mode:
    ; Set up segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Set up stack
    mov esp, 0x90000
    
    ; Jump to kernel at 0x10000
    jmp 0x08:0x10000

; Padding and boot signature
times 510-($-$$) db 0
dw 0xAA55
