#include "../drivers/serial/pl011.h"
#include "../arch/aarch64/arm_generic_timer.h"
#include "devicetree.h"
#include "driver.h"
#include "types.h"
#include "alloc.h"
#include "endian.h"
#include "device.h"
#include "str.h"
#include "memory.h"

static void print_hex64(uint64_t v) {
    char buf[17];
    for (int i = 15; i >= 0; i--) {
        int d = v & 0xF;
        buf[i] = (d < 10) ? ('0' + d) : ('a' + d - 10);
        v >>= 4;
    }
    buf[16] = 0;
    pl011_write(buf);
}

int kernel_main(uintptr_t dtb_ptr) {
    /*const struct device uart0 = {
    .name = "uart0",
    .device_type = "serial",
    .compatible = "ns16550a",
    .compatible_len = 9,
    .mmio_base = 0x3F201000,

    .mmio_size = 0x1000,
    .enabled = 1
    };
    pl011_init(&uart0);*/
    const struct fdt_header *hdr = parse_fdt_header(dtb_ptr);
    if (hdr == NULL){}
    parse_fdt(hdr);
    driver_registry_init();    
    driver_probe_all();
    pl011_write("Booting Tiny Kernel\n");
      

    // uint64_t memory = get_main_memory_base();
    uint32_t freq = get_clock_frequency();
    uint32_t interval = freq / 100;
    timer_arm(interval);
    //enable_core_timer_irq();
    asm volatile("msr DAIFClr, #2");
    return 0;
}

