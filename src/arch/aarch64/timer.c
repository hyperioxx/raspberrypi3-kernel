#include "types.h"

#define LOCAL_PERIPH_BASE 0x40000000UL
#define CORE0_TIMER_IRQ_CTRL (LOCAL_PERIPH_BASE + 0x40)

uint32_t get_clock_frequency() {
    uint32_t freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    return freq;
}

void timer_arm(uint32_t interval) {
    asm volatile("msr cntp_tval_el0, %0" :: "r"(interval));
    uint32_t ctl = 1; // ENABLE=1, IMASK=0
    asm volatile("msr cntp_ctl_el0, %0" :: "r"(ctl));
}


void enable_core_timer_irq(void) {
    volatile uint32_t *reg = (uint32_t *)CORE0_TIMER_IRQ_CTRL;
    *reg |= (1 << 1);  
    *reg |= (1 << 2);
}

