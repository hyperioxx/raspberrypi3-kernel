#ifndef TIMER_H
#define TIMER_H
#include "types.h"

uint32_t get_clock_frequency() ;
void timer_arm(uint32_t interval);
void enable_core_timer_irq(void);
#endif
