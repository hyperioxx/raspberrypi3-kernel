#include "uart.h"


int kernel_main(void) {
    uart_print("hello, world\n");
    while (1) {
    }
    return 0;
}

