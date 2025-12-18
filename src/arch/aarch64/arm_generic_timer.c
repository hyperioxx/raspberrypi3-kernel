#include "arm_generic_timer.h"
#include "types.h"
#include "device.h"


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

