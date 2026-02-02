#include "../arch/aarch64/arm_generic_timer.h"
#include "devicetree.h"
#include "driver.h"
#include "types.h"
#include "device.h"
#include "memory.h"
#include "console.h"

static void print_hex64(uint64_t v) {
    char buf[17];
    for (int i = 15; i >= 0; i--) {
        int d = v & 0xF;
        buf[i] = (d < 10) ? ('0' + d) : ('a' + d - 10);
        v >>= 4;
    }
    buf[16] = 0;
    console_write(buf);
}

int kernel_main(uintptr_t dtb_ptr) {
    // hard coded debug device for rpi3 
    const struct fdt_header *hdr = parse_fdt_header(dtb_ptr);
    if (hdr == NULL){}
    parse_fdt(hdr);
    driver_registry_init();    
    driver_probe_all();
    console_write("Booting Tiny Kernel\n");
      

    // uint64_t memory = get_main_memory_base();
    uint32_t freq = get_clock_frequency();
    uint32_t interval = freq / 100;
    timer_arm(interval);
    asm volatile("msr DAIFClr, #2");
    return 0;
}

