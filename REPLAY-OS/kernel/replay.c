#include <replay.h>
#include <trace.h>
#include <vga.h>
#include <serial.h>

static int replaying = 0;
static uint32_t replay_ptr = 0;
extern trace_event_t trace_buffer[1000];
extern uint32_t trace_count;

void replay_start() {
    replaying = 1;
    replay_ptr = 0;
    vga_print("Replay started.\n");
}

int is_replaying() {
    return replaying;
}

trace_event_t* replay_get_next() {
    if (replay_ptr >= trace_count) {
        replaying = 0;
        vga_print("Replay finished.\n");
        return 0;
    }
    return &trace_buffer[replay_ptr++];
}

void* replay_get_next_task(void* current_stack) {
    trace_event_t* event = replay_get_next();
    if (!event || event->type != EVENT_SCHEDULER) {
        return current_stack;
    }
    return current_stack;
}
