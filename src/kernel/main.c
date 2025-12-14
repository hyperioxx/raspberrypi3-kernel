#include "../drivers/serial/pl011.h"
#include "../arch/aarch64/timer.h"
#include "types.h"
#include "alloc.h"


//TODO: will need to come up with a better way to abstract arch's in future
int kernel_main(void) {
    pl011_write("Booting Tiny Kernel\n");
    uint32_t freq = get_clock_frequency();
    uint32_t interval = freq / 100;
    timer_arm(interval);
    enable_core_timer_irq();
    asm volatile("msr DAIFClr, #2");
    return 0;
}

