#include <trace.h>
#include <timer.h>
#include <serial.h>
#include <vga.h>

#define MAX_EVENTS 1000
trace_event_t trace_buffer[MAX_EVENTS];
uint32_t trace_head = 0;
uint32_t trace_count = 0;
static int recording = 0;

void trace_init() {
    trace_head = 0;
    trace_count = 0;
    recording = 0;
}

void trace_start() {
    recording = 1;
    vga_print("Tracing started.\n");
}

void trace_stop() {
    recording = 0;
    vga_print("Tracing stopped.\n");
}

void trace_record(event_type_t type, uint64_t payload) {
    if (!recording) return;

    trace_event_t* event = &trace_buffer[trace_head];
    event->type = type;
    event->tick = get_ticks();
    event->payload = payload;

    trace_head = (trace_head + 1) % MAX_EVENTS;
    if (trace_count < MAX_EVENTS) {
        trace_count++;
    }
}

void trace_dump() {
    vga_print("Trace Log:\n");
    for (uint32_t i = 0; i < trace_count; i++) {
        serial_print("Event Logged\n");
    }
}

int is_recording() {
    return recording;
}
