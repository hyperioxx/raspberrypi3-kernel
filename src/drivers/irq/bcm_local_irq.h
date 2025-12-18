#ifndef BCM_LOCAL_IRQ_H
#define BCM_LOCAL_IRQ_H

#include "types.h"
#include "device.h"

struct bcm_local_irq {
    volatile uint32_t *core0_timer_irq_ctrl;
    volatile uint32_t *core0_irq_source;
};

int bcm_local_irq_init(const struct device *dev);

uint32_t bcm_local_irq_source(void);
void bcm_local_enable_core_timer_irq(void);
#endif
