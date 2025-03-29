; Kernel.asm
BITS 32

section .text
    ; Multiboot header (must be within the first 8KB of the kernel image)
    align 4
    dd 0x1BADB002              ; magic number
    dd 0x00                    ; flags
    dd - (0x1BADB002 + 0x00)   ; checksum (magic + flags + checksum = 0)

; Export global labels
global start
global keyboard_handler
global page_fault_handler
global read_port
global write_port
global load_idt

; Import external C functions
extern kmain                     ; main kernel C function
extern keyboard_handler_main     ; C routine for keyboard interrupt
extern page_fault_handler_main   ; C routine for page fault exception

; -----------------------------
; Read from port: read_port(port)
read_port:
    mov edx, [esp + 4]  ; get port number from the stack
    in al, dx           ; read 8-bit value from port DX
    ret

; -----------------------------
; Write to port: write_port(port, data)
write_port:
    mov edx, [esp + 4]  ; get port number
    mov al, [esp + 8]   ; get data byte
    out dx, al          ; write to port DX
    ret

; -----------------------------
; Load the IDT: load_idt(idt_ptr)
load_idt:
    mov edx, [esp + 4]  ; get pointer to the IDT descriptor
    lidt [edx]          ; load IDT
    sti                 ; enable interrupts
    ret

; -----------------------------
; Keyboard Interrupt Handler Stub
keyboard_handler:
    call keyboard_handler_main
    iretd

; -----------------------------
; Page Fault Exception Handler Stub
; The CPU pushes an error code automatically. To pass that to our C function,
; we push it again from its location on the stack after saving registers.
page_fault_handler:
    pushad                      ; save general-purpose registers (8*4 = 32 bytes)
    push dword [esp + 32]       ; push error code (located 32 bytes above ESP now)
    call page_fault_handler_main
    add esp, 4                ; clean up the parameter (error code)
    popad                     ; restore registers
    iretd

; -----------------------------
; Kernel Entry Point
start:
    cli                             ; disable interrupts
    mov esp, stack_space + 8192     ; set stack pointer to top of our 8KB stack
    call kmain                      ; jump to C kernel main function
    hlt                             ; halt if kmain returns

section .bss
resb 8192                           ; 8KB stack space
stack_space:

