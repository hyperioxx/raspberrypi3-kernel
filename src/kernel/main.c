#include "../drivers/serial/pl011.h"
#include "../arch/aarch64/timer.h"
#include "devicetree.h"
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

//TODO: will need to come up with a better way to abstract arch's in future
int kernel_main(uintptr_t dtb_ptr) {
    const struct fdt_header *hdr = parse_fdt_header(dtb_ptr);
    if (hdr == NULL){}
    parse_fdt(hdr);
    
    const struct device *uart = device_find_compat("brcm,bcm2835-pl011");
    if (!uart) uart = device_find_compat("arm,pl011"); // some DTBs
    if (!uart) uart = device_find_compat("arm,pl011-axi");
    if (uart) {
        pl011_init(uart);
        pl011_write("Console: PL011 from DT\n");
    }

    pl011_write("Booting Tiny Kernel\n");
    uint64_t memory = get_main_memory_base();
    pl011_write("size ");
    print_hex64(memory);
    pl011_write("\n");


    //uint32_t freq = get_clock_frequency();
    //uint32_t interval = freq / 100;
    //timer_arm(interval);
    //enable_core_timer_irq();
    //asm volatile("msr DAIFClr, #2");
    return 0;
}

