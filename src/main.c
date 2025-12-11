#include "uart.h"

extern unsigned long _bss_start;
extern unsigned long _bss_end;

volatile unsigned int should_be_zero;

int kernel_main(void) {
    uart_print("hello, world\n");
    while (1) {
    }
    return 0;
}

