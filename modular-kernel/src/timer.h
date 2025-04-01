#ifndef TIMER_H
#define TIMER_H


#include "memory.h"
#include "kernel.h"
#include "irq.h"

void timer_phase(int hz);
void timer_install();
void timer_wait(int ticks);

#endif
