#ifndef PROC_H
#define PROC_H

#include <stdint.h>
#include "paging.h"

typedef enum {
    PROC_RUNNING,
    PROC_READY,
    PROC_WAITING,
    PROC_TERMINATED
} proc_state_t;

typedef struct context {
    uint32_t eip, esp, ebp;
    uint32_t eax, ebx, ecx, edx, esi, edi;
    uint32_t eflags;
} context_t;

typedef struct process {
    uint32_t pid;
    proc_state_t state;
    context_t context;
    uint32_t* page_directory;
    struct process *next;
} process_t;

extern process_t* current_proc;

void proc_init(void);
process_t* proc_create(void (*entry)());
void proc_schedule(void);
void proc_yield(void);
void switch_context(context_t* old, context_t* new);

#endif
