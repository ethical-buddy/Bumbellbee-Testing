#include <trace.h>
#include <ide.h>
#include <vga.h>

extern trace_event_t trace_buffer[1000];
extern uint32_t trace_count;

void trace_save() {
    vga_print("Saving trace to disk...\n");
    
    uint8_t buffer[512];
    for(int i=0; i<512; i++) buffer[i] = 0;
    *((uint32_t*)buffer) = trace_count;
    ide_write_sector(1000, buffer);
    
    uint32_t sectors_needed = (trace_count * sizeof(trace_event_t)) / 512 + 1;
    for (uint32_t i = 0; i < sectors_needed; i++) {
        ide_write_sector(1001 + i, (uint8_t*)trace_buffer + (i * 512));
    }
    vga_print("Trace saved.\n");
}

void trace_load() {
    vga_print("Loading trace from disk...\n");
    uint8_t buffer[512];
    ide_read_sector(1000, buffer);
    trace_count = *((uint32_t*)buffer);
    
    uint32_t sectors_needed = (trace_count * sizeof(trace_event_t)) / 512 + 1;
    for (uint32_t i = 0; i < sectors_needed; i++) {
        ide_read_sector(1001 + i, (uint8_t*)trace_buffer + (i * 512));
    }
    vga_print("Trace loaded.\n");
}
