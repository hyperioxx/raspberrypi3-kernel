#ifndef ARM_GENERIC_TIMER_H
#define ARM_GENERIC_TIMER_H
#include "types.h"
#include "device.h"

uint32_t core0_irq_source(void);
uint32_t get_clock_frequency() ;
void timer_arm(uint32_t interval);
void enable_core_timer_irq(void);
#endif
