#include <syscall.h>
#include <serial.h>
#include <vga.h>
#include <idt.h>

#include <trace.h>

void syscall_handler(interrupt_frame_t* frame) {
    uint64_t syscall_num = frame->rax;
    trace_record(EVENT_SYSCALL, syscall_num);
    
    if (syscall_num == 1) { // sys_write
        const char* str = (const char*)frame->rdi;
        vga_print(str);
        serial_print(str);
    }
}

void syscall_init() {
    extern void isr_stub_128();
    idt_set_descriptor(128, isr_stub_128, 0x8E);
}
