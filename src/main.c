#include "uart.h"

extern unsigned long _bss_start;
extern unsigned long _bss_end;

volatile unsigned int should_be_zero;

int kernel_main(void) {
    // we expect this to be 0 if .bss was cleared
    // just a simple check using uart_print for now
    if (should_be_zero == 0) {
        uart_print("BSS OK: should_be_zero == 0\n");
    } else {
        uart_print("BSS BROKEN: should_be_zero != 0\n");
    }
    uart_print("hello, world\n");
    while (1) {
    }
    return 0;
}

