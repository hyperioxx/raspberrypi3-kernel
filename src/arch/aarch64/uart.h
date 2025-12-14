#ifndef UART_H
#define UART_H


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
#define UART_BASE 0x3F201000
#define UART_DR  (UART_BASE + 0x00)
#define UART_FR  (UART_BASE + 0x18)
#define UART_CR  (UART_BASE + 0x30)

extern volatile unsigned int * const UART0_DR;
extern volatile unsigned int * const UART0_FR;
extern volatile unsigned int * const UART0_CR;

void uart_print(char *);

#endif
