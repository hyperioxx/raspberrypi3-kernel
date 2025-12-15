#include "pl011.h" 

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
#define UART_DR   0x00
#define UART_FR   0x18
#define UART_CR   0x30

static struct pl011 uart0;

void pl011_init(uintptr_t base)
{
    uart0.dr = (volatile uintptr_t *)(base + UART_DR);
    uart0.fr = (volatile uintptr_t *)(base + UART_FR);
    uart0.cr = (volatile uintptr_t *)(base + UART_CR);
}

void pl011_write(const char *s)
{
    while (*s) {
        while (uart0.fr && (*uart0.fr & (1u << 5))) {}
        *uart0.dr = (uint32_t)(unsigned char)*s++;
    }
}

