#ifndef PL011_H
#define PL011_H
#include "types.h"

struct pl011 {
    volatile uint32_t *base; 
};

void pl011_write(struct pl011 *, char *);

void pl011_init(struct pl011 *uart, uintptr_t base);
#endif
