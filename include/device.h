#ifndef DEVICE_H
#define DEVICE_H

#include "types.h"

struct device {
    const char *name;        // node name
    const char *compatible;  // first compatible string
    uint64_t mmio_base;
    uint64_t mmio_size;
    int enabled;
};

void device_register(const struct device *d);
size_t device_count(void);
const struct device *device_get(size_t i);

#endif
