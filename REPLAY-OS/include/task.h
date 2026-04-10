#ifndef TASK_H
#define TASK_H

#include <stdint.h>

typedef struct {
    uint64_t r15, r14, r13, r12, rbp, rbx, r11, r10, r9, r8, rax, rcx, rdx, rsi, rdi;
    uint64_t rip, cs, rflags, rsp, ss;
} __attribute__((packed)) context_t;

typedef enum {
    TASK_RUNNING,
    TASK_READY,
    TASK_SLEEPING,
    TASK_EXITED
} task_state_t;

typedef struct task {
    uint32_t pid;
    task_state_t state;
    uint64_t* stack_ptr;
    struct task* next;
} task_t;

void scheduler_init();
void* scheduler_schedule(void* stack_ptr);
void task_create(void (*entry_point)());

#endif
