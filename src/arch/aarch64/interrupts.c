#include "../../drivers/serial/pl011.h"
#include "interrupts.h"
#include "timer.h"
#include "types.h"

void handle_interrupt_c(struct trap_frame *tf) {
    uint32_t freq = get_clock_frequency();
    uint32_t interval = freq/100 ;
    timer_arm(interval);
    //pl011_write("Interrupt Called\n");

}
