[bits 64]
global idt_load
idt_load:
    lidt [rdi]
    ret

%macro isr_err_stub 1
isr_stub_%1:
    push %1
    jmp isr_common_stub
%endmacro

%macro isr_no_err_stub 1
isr_stub_%1:
    push 0
    push %1
    jmp isr_common_stub
%endmacro

extern isr_handler
extern syscall_handler
isr_common_stub:
    push rdi
    push rsi
    push rdx
    push rcx
    push rax
    push r8
    push r9
    push r10
    push r11
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    mov rdi, rsp
    
    ; Check if it's syscall (128)
    cmp qword [rsp + 120], 128
    je .syscall
    
    call isr_handler
    jmp .done

.syscall:
    call syscall_handler

.done:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    pop r11
    pop r10
    pop r9
    pop r8
    pop rax
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    add rsp, 16
    iretq

global isr_stub_128
isr_no_err_stub 128

; Exceptions
global isr_stub_0
isr_no_err_stub 0
global isr_stub_1
isr_no_err_stub 1
global isr_stub_2
isr_no_err_stub 2
global isr_stub_3
isr_no_err_stub 3
global isr_stub_4
isr_no_err_stub 4
global isr_stub_5
isr_no_err_stub 5
global isr_stub_6
isr_no_err_stub 6
global isr_stub_7
isr_no_err_stub 7
global isr_stub_8
isr_err_stub    8
global isr_stub_9
isr_no_err_stub 9
global isr_stub_10
isr_err_stub    10
global isr_stub_11
isr_err_stub    11
global isr_stub_12
isr_err_stub    12
global isr_stub_13
isr_err_stub    13
global isr_stub_14
isr_err_stub    14
global isr_stub_15
isr_no_err_stub 15
global isr_stub_16
isr_no_err_stub 16
global isr_stub_17
isr_err_stub    17
global isr_stub_18
isr_no_err_stub 18
global isr_stub_19
isr_no_err_stub 19
global isr_stub_20
isr_no_err_stub 20
global isr_stub_21
isr_err_stub    21
global isr_stub_22
isr_no_err_stub 22
global isr_stub_23
isr_no_err_stub 23
global isr_stub_24
isr_no_err_stub 24
global isr_stub_25
isr_no_err_stub 25
global isr_stub_26
isr_no_err_stub 26
global isr_stub_27
isr_no_err_stub 27
global isr_stub_28
isr_no_err_stub 28
global isr_stub_29
isr_err_stub    29
global isr_stub_30
isr_err_stub    30
global isr_stub_31
isr_no_err_stub 31

; IRQs
%macro irq_stub 2
global irq_stub_%1
irq_stub_%1:
    push 0
    push %2
    jmp irq_common_stub
%endmacro

extern irq_handler
extern scheduler_schedule
irq_common_stub:
    push rdi
    push rsi
    push rdx
    push rcx
    push rax
    push r8
    push r9
    push r10
    push r11
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    mov rdi, rsp
    call irq_handler

    ; Context Switch if it was a timer interrupt (int_no 32)
    cmp qword [rsp + 120], 32
    jne .no_switch
    
    mov rdi, rsp
    call scheduler_schedule
    mov rsp, rax

.no_switch:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    pop r11
    pop r10
    pop r9
    pop r8
    pop rax
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    add rsp, 16
    iretq

irq_stub 0, 32
irq_stub 1, 33
irq_stub 2, 34
irq_stub 3, 35
irq_stub 4, 36
irq_stub 5, 37
irq_stub 6, 38
irq_stub 7, 39
irq_stub 8, 40
irq_stub 9, 41
irq_stub 10, 42
irq_stub 11, 43
irq_stub 12, 44
irq_stub 13, 45
irq_stub 14, 46
irq_stub 15, 47
