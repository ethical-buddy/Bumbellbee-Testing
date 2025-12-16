; idt.asm - Interrupt Descriptor Table and ISR stubs
section .data

align 8
idt_start:
    times 256 dq 0  ; 256 IDT entries (8 bytes each)
idt_end:

idt_descriptor:
    dw idt_end - idt_start - 1  ; Size
    dd idt_start                ; Offset

section .text
global idt_load
global idt_set_gate
global idt_descriptor

; External interrupt handlers
extern isr_keyboard_handler
extern isr_default_handler

; Load IDT
idt_load:
    lidt [idt_descriptor]
    ret

; Set an IDT gate
; Parameters: stack-based
; idt_set_gate(uint8_t num, uint32_t handler, uint16_t selector, uint8_t flags)
idt_set_gate:
    push ebp
    mov ebp, esp
    push ebx
    
    mov eax, [ebp + 8]      ; num
    mov ebx, [ebp + 12]     ; handler
    mov cx, [ebp + 16]      ; selector
    mov dx, [ebp + 20]      ; flags
    
    ; Calculate IDT entry address
    shl eax, 3              ; num * 8
    add eax, idt_start
    
    ; Set handler address (low 16 bits)
    mov word [eax], bx
    
    ; Set selector
    mov word [eax + 2], cx
    
    ; Set reserved byte (always 0)
    mov byte [eax + 4], 0
    
    ; Set flags
    mov byte [eax + 5], dl
    
    ; Set handler address (high 16 bits)
    shr ebx, 16
    mov word [eax + 6], bx
    
    pop ebx
    pop ebp
    ret

; ISR stub macros and handlers
global isr_stub_keyboard
global isr_stub_timer

; Keyboard ISR (IRQ1 = interrupt 33)
isr_stub_keyboard:
    pushad
    cld
    call isr_keyboard_handler
    mov al, 0x20
    out 0x20, al            ; Send EOI to PIC
    popad
    iret

; Timer ISR (IRQ0 = interrupt 32) - we'll use this for PIC initialization
isr_stub_timer:
    pushad
    mov al, 0x20
    out 0x20, al            ; Send EOI to PIC
    popad
    iret
