/* proc.c */
#include "proc.h"
#include "paging.h"
#include "mm.h"
#include "kernel.h"

static process_t *proc_list = 0;
process_t *current_proc = 0;
static int next_pid = 1;

void proc_init(void) {
    proc_list = 0;
    current_proc = 0;
}

process_t* proc_create(void (*entry)()) {
    process_t *proc = (process_t*) mm_alloc(sizeof(process_t));
    if (!proc) {
        kprint("proc_create: Out of memory");
        return 0;
    }
    proc->pid = next_pid++;
    proc->state = PROC_READY;
    proc->context.eip = (uint32_t) entry;
    void *stack = mm_alloc(4096);
    if (!stack) {
        kprint("proc_create: Out of memory for stack");
        return 0;
    }
    proc->context.esp = (uint32_t) stack + 4096;
    proc->context.ebp = proc->context.esp;
    proc->context.eflags = 0x202;
    proc->page_directory = create_page_directory();

    proc->next = 0;
    if (!proc_list)
        proc_list = proc;
    else {
        process_t *p = proc_list;
        while (p->next)
            p = p->next;
        p->next = proc;
    }
    return proc;
}

void proc_schedule(void) {
    if (!proc_list)
        return;
    
    process_t *next = current_proc ? current_proc->next : proc_list;
    if (!next)
        next = proc_list;
    
    while (next->state != PROC_READY && next != current_proc) {
        next = next->next ? next->next : proc_list;
    }

    if (next != current_proc) {
        process_t *prev = current_proc;
        current_proc = next;
        current_proc->state = PROC_RUNNING;
        switch_context(&prev->context, &current_proc->context);
    }
}

void proc_yield(void) {
    proc_schedule();
}
