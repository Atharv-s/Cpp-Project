#ifndef TIMER_H
#define TIMER_H
#include "kernel.h"

void timer_install(uint32_t frequency_hz);
uint32_t timer_ticks();
void sleep_ticks(uint32_t ticks);

#endif
