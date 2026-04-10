#include <task.h>
#include <pmm.h>
#include <serial.h>
#include <vga.h>

static task_t* current_task = NULL;
static task_t* task_list = NULL;
static uint32_t next_pid = 1;

void scheduler_init() {
    // Current execution becomes the first task
    task_t* task = (task_t*)0x110000; // Safe address within mapped 2MB
    task->pid = next_pid++;
    task->state = TASK_RUNNING;
    task->next = task;
    task_list = task;
    current_task = task;
}

void task_create(void (*entry_point)()) {
    task_t* task = (task_t*)(0x110000 + (next_pid * sizeof(task_t)));
    task->pid = next_pid++;
    task->state = TASK_READY;
    
    uint64_t* stack = (uint64_t*)pmm_alloc_frame();
    stack += 511; // End of page
    
    // Initial context
    *--stack = 0x20;          // SS (DATA_SEG_64)
    *--stack = (uint64_t)stack + 8; // RSP
    *--stack = 0x202;         // RFLAGS (IF=1)
    *--stack = 0x18;          // CS (CODE_SEG_64)
    *--stack = (uint64_t)entry_point; // RIP
    
    // General purpose registers
    for (int i = 0; i < 15; i++) {
        *--stack = 0;
    }
    
    task->stack_ptr = stack;
    task->next = task_list->next;
    task_list->next = task;
}

task_t* scheduler_next() {
    task_t* task = current_task->next;
    while (task->state != TASK_READY && task->state != TASK_RUNNING) {
        task = task->next;
    }
    return task;
}

#include <trace.h>

void* scheduler_schedule(void* stack_ptr) {
    if (current_task) {
        current_task->stack_ptr = (uint64_t*)stack_ptr;
    }
    
    current_task = scheduler_next();
    trace_record(EVENT_SCHEDULER, current_task->pid);
    
    return current_task->stack_ptr;
}
