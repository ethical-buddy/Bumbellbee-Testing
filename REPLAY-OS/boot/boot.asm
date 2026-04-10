[org 0x7c00]
[bits 16]

KERNEL_OFFSET equ 0x8000 ; The memory offset to which we will load our kernel

start:
    ; Set up segments
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    mov [BOOT_DRIVE], dl ; BIOS stores our boot drive in dl

    mov bx, MSG_REAL_MODE
    call print_string

    call load_kernel ; Read the kernel from disk

    jmp KERNEL_OFFSET ; Jump to the address of our loaded kernel code

%include "boot/print.asm"

[bits 16]
load_kernel:
    mov bx, MSG_LOAD_KERNEL
    call print_string

    mov bx, KERNEL_OFFSET ; Read from disk and store in 0x8000
    mov dh, 63            ; Load 63 sectors (Stage 2 + Kernel)
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret

[bits 16]
disk_load:
    pusha
    push dx

    mov ah, 0x02 ; BIOS read sector function
    mov al, dh   ; Number of sectors to read
    mov cl, 0x02 ; Start reading from second sector (0x01 is boot sector)
    mov ch, 0x00 ; Cylinder 0
    mov dh, 0x00 ; Head 0

    ; dl (drive number) is set before call

    int 0x13      ; BIOS interrupt
    jc disk_error ; if error (carry flag set)

    pop dx
    cmp al, dh    ; BIOS sets al to sectors read
    jne disk_error
    popa
    ret

disk_error:
    mov bx, DISK_ERROR_MSG
    call print_string
    mov dh, ah ; ah has error code
    ; call print_hex ; optional
    jmp $

; Variables
BOOT_DRIVE db 0
MSG_REAL_MODE db "Started in 16-bit Real Mode", 0x0D, 0x0A, 0
MSG_LOAD_KERNEL db "Loading kernel into memory", 0x0D, 0x0A, 0
DISK_ERROR_MSG db "Disk read error", 0x0D, 0x0A, 0

; Padding and magic number
times 510-($-$$) db 0
dw 0xaa55
