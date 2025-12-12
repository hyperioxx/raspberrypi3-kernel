#include "aarch64/uart.h"
#include "types.h"
#include "alloc.h"

int kernel_main(void) {
    uart_print("Tiny Kernel");
    kalloc(20);
    while (1) {
    }
    return 0;
}

