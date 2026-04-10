#include <shell.h>
#include <vga.h>
#include <serial.h>
#include <timer.h>
#include <pmm.h>
#include <task.h>
#include <trace.h>
#include <replay.h>
#include <workload.h>

static char input_buffer[256];
static int buffer_index = 0;

extern char keyboard_getc();
extern void trace_start();
extern void trace_stop();
extern void trace_save();
extern void trace_load();

void shell_run() {
    vga_print("REPLAY-OS Research Console\n");
    vga_print("> ");
    
    while(1) {
        char c = keyboard_getc();
        if (c == 0) continue;
        
        if (c == '\n') {
            vga_print("\n");
            input_buffer[buffer_index] = '\0';
            
            if (buffer_index > 0) {
                if (input_buffer[0] == 't') {
                    if (input_buffer[6] == 's') trace_start();
                    else if (input_buffer[6] == 't') trace_stop();
                    else if (input_buffer[6] == 'v') trace_save();
                } else if (input_buffer[0] == 'r') {
                    if (input_buffer[7] == 'd') trace_load();
                    else replay_start();
                } else if (input_buffer[0] == 'w') {
                    if (input_buffer[9] == 'i') workload_io();
                    else workload_cpu();
                } else if (input_buffer[0] == 'a') {
                    attack_sim_overflow();
                } else if (input_buffer[0] == 'h') {
                    vga_print("trace start/stop/save, replay, workload io/cpu, attack\n");
                }
            }
            
            buffer_index = 0;
            vga_print("> ");
        } else if (c == '\b') {
            if (buffer_index > 0) {
                buffer_index--;
                vga_print("\b \b");
            }
        } else {
            if (buffer_index < 255) {
                input_buffer[buffer_index++] = c;
                vga_putc(c);
            }
        }
    }
}
