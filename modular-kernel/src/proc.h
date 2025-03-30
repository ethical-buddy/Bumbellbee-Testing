#ifndef PROC_H
#define PROC_H

#include <stdint.h>
#include <stddef.h>

#define MAX_PROCESSES 32

typedef enum {
    PROC_RUNNING,
    PROC_READY,
    PROC_WAITING,
    PROC_TERMINATED
} proc_state_t;

typedef struct context {
    uint32_t eip;
    uint32_t esp;
    uint32_t ebp;
    uint32_t eax, ebx, ecx, edx;
    uint32_t esi, edi;
    uint32_t eflags;
} context_t;

typedef struct process {
    int pid;
    proc_state_t state;
    context_t context;
    // For future use: pointer to a process-specific page directory.
    uint32_t *page_directory;
    struct process *next;
} process_t;

void proc_init(void);
process_t* proc_create(void (*entry)());
void proc_schedule(void);
void proc_yield(void);

#endif

