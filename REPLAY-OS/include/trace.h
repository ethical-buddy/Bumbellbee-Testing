#ifndef TRACE_H
#define TRACE_H

#include <stdint.h>

typedef enum {
    EVENT_TIMER,
    EVENT_KEYBOARD,
    EVENT_SYSCALL,
    EVENT_SCHEDULER
} event_type_t;

typedef struct {
    uint32_t type;
    uint32_t pid;
    uint64_t tick;
    uint64_t payload;
} trace_event_t;

void trace_init();
void trace_record(event_type_t type, uint64_t payload);
void trace_dump();

#endif
