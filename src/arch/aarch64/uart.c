#include "uart.h" 
volatile unsigned int * const UART0_DR = (unsigned int*)UART_DR; 
volatile unsigned int * const UART0_FR = (unsigned int*)UART_FR; 
volatile unsigned int * const UART0_CR = (unsigned int*)UART_CR; 


void uart_print(char *string) {
    for (int i = 0; string[i] != '\0'; i++) { 
        while (*UART0_FR & (1 << 5)) {}
        *UART0_DR = string[i]; 
    } 
}
