#pragma once
#include "types.h"
#include "device.h"

int gic400_init(const struct device *dev);

uint32_t gic400_ack_irq(void);
void gic400_eoi(uint32_t irq);

