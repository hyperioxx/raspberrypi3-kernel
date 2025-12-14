#ifndef PL011_H
#define PL011_H
#include "types.h"

struct pl011 {
    volatile uintptr_t *dr;
    volatile uintptr_t *fr;
    volatile uintptr_t *cr;
};

void pl011_init(uintptr_t base);
void pl011_write(const char *s);


#endif
