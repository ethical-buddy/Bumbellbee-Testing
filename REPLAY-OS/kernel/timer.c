#include <timer.h>
#include <io.h>
#include <serial.h>

#include <trace.h>

static uint64_t ticks = 0;

void timer_init(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;

    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

void timer_callback() {
    ticks++;
    trace_record(EVENT_TIMER, ticks);
}

uint64_t get_ticks() {
    return ticks;
}
