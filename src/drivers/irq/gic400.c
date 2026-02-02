#include "gic400.h"
#include "device.h"
#include "types.h"

#define GICD_CTLR      0x000
#define GICC_CTLR      0x000
#define GICC_PMR       0x004
#define GICC_IAR       0x00C
#define GICC_EOIR      0x010
#define GICD_ISENABLER0 0x100
#define GIC_CPU_OFFSET 0x1000

static volatile uint32_t *gicd_ctlr;
static volatile uint32_t *gicc_ctlr;
static volatile uint32_t *gicc_pmr;
static volatile uint32_t *gicc_iar;
static volatile uint32_t *gicc_eoir;
static volatile uint32_t *gicd_isenabler0;

int gic400_init(const struct device *dev)
{
    uintptr_t base = dev->mmio_base;

    gicd_ctlr  = (volatile uint32_t *)(base + GICD_CTLR);
    gicc_ctlr  = (volatile uint32_t *)(base + GIC_CPU_OFFSET + GICC_CTLR);
    gicc_pmr   = (volatile uint32_t *)(base + GIC_CPU_OFFSET + GICC_PMR);
    gicc_iar   = (volatile uint32_t *)(base + GIC_CPU_OFFSET + GICC_IAR);
    gicc_eoir  = (volatile uint32_t *)(base + GIC_CPU_OFFSET + GICC_EOIR);
    gicd_isenabler0 = (volatile uint32_t *)(base + GICD_ISENABLER0);
  
    /* Enable distributor */
    *gicd_ctlr = 1;

    /* Enable CPU interface */
    *gicc_pmr  = 0xFF;  // allow all priorities
    *gicc_ctlr = 1;

    /* Enable PPI 30 (ARM physical timer) */
    *gicd_isenabler0 = (1u << 30);

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

