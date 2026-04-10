#include <workload.h>
#include <ide.h>
#include <vga.h>

void workload_io() {
    vga_print("Running IO workload...\n");
    uint8_t buffer[512];
    for (int i = 0; i < 512; i++) buffer[i] = i;
    for (int i = 0; i < 10; i++) {
        ide_write_sector(5000 + i, buffer);
    }
    vga_print("IO workload done.\n");
}

void workload_cpu() {
    vga_print("Running CPU workload...\n");
    for (volatile int i = 0; i < 100000000; i++);
    vga_print("CPU workload done.\n");
}

void attack_sim_overflow() {
    vga_print("Simulating attack: Exception Trigger\n");
    int* bad_ptr = (int*)0xFFFFFFFFFFFFFFFF;
    *bad_ptr = 123; 
}
