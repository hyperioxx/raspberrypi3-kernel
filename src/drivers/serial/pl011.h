#ifndef PL011_H
#define PL011_H
#include "types.h"
#include "device.h"

struct pl011 {
    volatile uintptr_t *dr;
    volatile uintptr_t *fr;
    volatile uintptr_t *cr;
};

int pl011_init(const struct device *);
void pl011_write(const char *s);


#endif
