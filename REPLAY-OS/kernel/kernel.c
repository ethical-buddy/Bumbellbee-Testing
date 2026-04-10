#include <vga.h>
#include <serial.h>
#include <idt.h>
#include <timer.h>
#include <keyboard.h>
#include <pmm.h>
#include <task.h>
#include <syscall.h>
#include <shell.h>
#include <trace.h>

void kernel_main() {
    init_serial();
    vga_init();
    serial_print("REPLAY-OS 64-bit Booting...\n");
    
    idt_init();
    pmm_init(128 * 1024 * 1024);
    syscall_init();
    trace_init();
    
    scheduler_init();
    timer_init(100);
    keyboard_init();
    
    serial_print("Kernel Initialized. Starting Shell...\n");
    shell_run();
    
    while(1) {
        __asm__ volatile ("hlt");
    }
}
