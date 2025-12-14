#include "pl011.h" 
#include <stdint.h>

//TODO: these are specific raspberry pi 3 memory mapped address, I would need to add the ability to set per platform
/* see section 13.4 Register View in docs/BCM2837-ARM-Peripherals.pdf
0x00     DR        Data Register
0x04     RSR/ESR   Receive Status / Error Clear
0x18     FR        Flag Register
0x24     IBRD      Integer Baud Rate Divisor
0x28     FBRD      Fractional Baud Rate Divisor
0x2C     LCRH      Line Control Register
0x30     CR        Control Register
0x44     ICR       Interrupt Clear Register
 */
//#define UART_BASE 0x3F201000
//
#define UART_DR  (UART_BASE + 0x00)
#define UART_FR  (UART_BASE + 0x18)
#define UART_CR  (UART_BASE + 0x30)

volatile unsigned int * const UART0_DR = (unsigned int*)UART_DR; 
volatile unsigned int * const UART0_FR = (unsigned int*)UART_FR; 
volatile unsigned int * const UART0_CR = (unsigned int*)UART_CR;

void pl011_init(struct pl011 *uart, uintptr_t base){
    uart->base = (volatile uint32_t *) base;
}

void pl011_write(struct pl011 *uart, char *string) {
    for (int i = 0; string[i] != '\0'; i++) { 
        while (*UART0_FR & (1 << 5)) {}
        *UART0_DR = string[i]; 
    } 
}
