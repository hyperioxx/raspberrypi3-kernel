#include "device.h"

#define MAX_DEVICES 256
static struct device g_devs[MAX_DEVICES];
static size_t g_dev_count = 0;

void device_register(const struct device *d) {
    if (!d) return;
    if (g_dev_count >= MAX_DEVICES) return;
    g_devs[g_dev_count++] = *d; // struct copy
}

size_t device_count(void) {
    return g_dev_count;
}

const struct device *device_get(size_t i) {
    if (i >= g_dev_count) return 0;
    return &g_devs[i];
}

