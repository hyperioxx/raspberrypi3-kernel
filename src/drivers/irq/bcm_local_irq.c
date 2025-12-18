#include "bcm_local_irq.h"
#include "device.h"
#include "types.h"
#include "../serial/pl011.h"

#define CORE0_TIMER_IRQ_CTRL  0x40
#define CORE0_IRQ_SOURCE      0x60

static struct bcm_local_irq irqc;

int bcm_local_irq_init(const struct device *dev)
{
    pl011_write("I GET HERE\n");
    irqc.core0_timer_irq_ctrl =
        (volatile uint32_t *)(dev->mmio_base + CORE0_TIMER_IRQ_CTRL);

    irqc.core0_irq_source =
        (volatile uint32_t *)(dev->mmio_base + CORE0_IRQ_SOURCE);

    bcm_local_enable_core_timer_irq();
    return 0;
}

uint32_t bcm_local_irq_source(void)
{
    if (!irqc.core0_irq_source)
        return 0;

    return *irqc.core0_irq_source;
}

void bcm_local_enable_core_timer_irq(void)
{
    if (!irqc.core0_timer_irq_ctrl)
        return;

    *irqc.core0_timer_irq_ctrl |= (1u << 1);
    *irqc.core0_timer_irq_ctrl |= (1u << 2);
}

