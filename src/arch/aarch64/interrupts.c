#include "interrupts.h"
#include "arm_generic_timer.h"
#include "types.h"
#include "../../drivers/serial/pl011.h"


void handle_interrupt_c(struct trap_frame *tf, uint64_t type) {
     switch (type) {
        case 0: handle_sync_interrupt(tf); break;
        case 1: handle_irq_interrupt(tf); break;
        case 2: handle_fiq_interrupt(tf); break;
        case 3: handle_serror_interrupt(tf); break;
    }
}

void handle_sync_interrupt(struct trap_frame *tf) {}

void handle_irq_interrupt(struct trap_frame *tf) {
    uint32_t freq = get_clock_frequency();
    uint32_t interval = freq/100 ;
    timer_arm(interval);
    pl011_write("Interrupt Called\n");
    asm volatile("msr DAIFClr, #2");

}
void handle_fiq_interrupt(struct trap_frame *tf) {}
void handle_serror_interrupt(struct trap_frame *tf) {}
