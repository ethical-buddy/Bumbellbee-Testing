#ifndef REPLAY_H
#define REPLAY_H

#include <stdint.h>
#include <trace.h>

void replay_start();
void replay_next_event();
int is_replaying();

#endif
