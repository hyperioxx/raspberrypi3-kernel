#include "uart.h"
#include "interrupts.h"

void handle_interrupt_c(struct trap_frame *tf) {
   uart_print("Interrupt Called\n");
}
