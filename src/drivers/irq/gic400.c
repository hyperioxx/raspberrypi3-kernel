#include "gic400.h"
#include "device.h"
#include "types.h"

/* GICv2 register offsets */
#define GICD_CTLR      0x000
#define GICC_CTLR      0x000
#define GICC_PMR       0x004
#define GICC_IAR       0x00C
#define GICC_EOIR      0x010

/* Pi 4: GIC distributor is at base, CPU interface is +0x2000 */
#define GIC_CPU_OFFSET 0x2000

static volatile uint32_t *gicd_ctlr;
static volatile uint32_t *gicc_ctlr;
static volatile uint32_t *gicc_pmr;
static volatile uint32_t *gicc_iar;
static volatile uint32_t *gicc_eoir;

int gic400_init(const struct device *dev)
{
    uintptr_t base = dev->mmio_base;

    gicd_ctlr  = (volatile uint32_t *)(base + GICD_CTLR);
    gicc_ctlr  = (volatile uint32_t *)(base + GIC_CPU_OFFSET + GICC_CTLR);
    gicc_pmr   = (volatile uint32_t *)(base + GIC_CPU_OFFSET + GICC_PMR);
    gicc_iar   = (volatile uint32_t *)(base + GIC_CPU_OFFSET + GICC_IAR);
    gicc_eoir  = (volatile uint32_t *)(base + GIC_CPU_OFFSET + GICC_EOIR);

    /* Enable distributor */
    *gicd_ctlr = 1;

    /* Enable CPU interface */
    *gicc_pmr  = 0xFF;  // allow all priorities
    *gicc_ctlr = 1;

    return 0;
}

uint32_t gic400_ack_irq(void)
{
    return *gicc_iar & 0x3FF;  // interrupt ID
}

void gic400_eoi(uint32_t irq)
{
    *gicc_eoir = irq;
}

