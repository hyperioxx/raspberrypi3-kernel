#include "device.h"
#include "../drivers/serial/pl011.h"

#define MAX_DEVICES 256
static struct device devices[MAX_DEVICES];
static size_t devices_count = 0;


void device_register(const struct device *dev) {
    if (!dev) return;
    if (devices_count >= MAX_DEVICES) return;

    struct device *dst = &devices[devices_count++];

    dst->name       = dev->name;
    dst->compatible = dev->compatible;
    dst->compatible_len = dev->compatible_len;
    dst->mmio_base  = dev->mmio_base;
    dst->mmio_size  = dev->mmio_size;
    dst->enabled    = dev->enabled;
}

size_t device_count(void) {
    return devices_count;
}

const struct device *device_get(size_t i) {
    if (i >= devices_count) return 0;
    return &devices[i];
}

static int streq(const char *a, const char *b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return (*a == 0 && *b == 0);
}

static int compat_has(const char *list, uint32_t len, const char *want) {
    if (!list || !len) return 0;

    const char *p = list;
    const char *end = list + len;

    while (p < end && *p) {
        const char *a = p, *b = want;
        while (*a && *b && *a == *b) { a++; b++; }
        if (*a == 0 && *b == 0) return 1;

        while (p < end && *p) p++;  
        p++;                      
    }
    return 0;
}

const struct device *device_find_compat(const char *compat) {
    for (size_t i = 0; i < device_count(); i++) {
        const struct device *dev = device_get(i);
        if (!dev || !dev->enabled) continue;
        if (compat_has(dev->compatible, dev->compatible_len, compat))
            return dev;
    }
    return 0;
}

