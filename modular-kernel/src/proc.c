#include "proc.h"
#include "mm.h"
#include "kernel.h"

static process_t *proc_list = 0;
static process_t *current_proc = 0;
static int next_pid = 1;

void proc_init(void)
{
    proc_list = 0;
    current_proc = 0;
}

process_t* proc_create(void (*entry)())
{
    process_t *proc = (process_t*) mm_alloc(sizeof(process_t));
    if (!proc) {
        kprint("proc_create: Out of memory");
        return 0;
    }
    proc->pid = next_pid++;
    proc->state = PROC_READY;
    proc->context.eip = (uint32_t) entry;
    void *stack = mm_alloc(4096);  // Allocate a 4KB stack
    if (!stack) {
        kprint("proc_create: Out of memory for stack");
        return 0;
    }
    proc->context.esp = (uint32_t) stack + 4096;
    proc->context.ebp = proc->context.esp;
    proc->context.eax = proc->context.ebx = proc->context.ecx = proc->context.edx = 0;
    proc->context.esi = proc->context.edi = 0;
    proc->context.eflags = 0x202;  // IF flag set
    proc->page_directory = 0;      // For now, processes share kernel paging

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

void proc_schedule(void)
{
    if (!proc_list)
        return;
    if (!current_proc)
        current_proc = proc_list;
    else
        current_proc = current_proc->next ? current_proc->next : proc_list;
    if (current_proc->state == PROC_READY) {
        current_proc->state = PROC_RUNNING;
        void (*entry)() = (void (*)()) current_proc->context.eip;
        entry();
        current_proc->state = PROC_TERMINATED;
    }
}

void proc_yield(void)
{
    proc_schedule();
    // In a complete kernel, a proper context switch would occur here.
}

