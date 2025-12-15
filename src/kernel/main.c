#include "../drivers/serial/pl011.h"
#include "../arch/aarch64/timer.h"
#include "devicetree.h"
#include "types.h"
#include "alloc.h"
#include "endian.h"


//TODO: will need to come up with a better way to abstract arch's in future
int kernel_main(uintptr_t dtb_ptr) {
    const struct fdt_header *hdr = parse_fdt_header(dtb_ptr);
    if (hdr == NULL){
        
    }
    uintptr_t base = 0x3F201000;
    pl011_init(base);
    pl011_write("Booting Tiny Kernel\n");
    uint32_t freq = get_clock_frequency();
    uint32_t interval = freq / 100;
    timer_arm(interval);
    enable_core_timer_irq();
    asm volatile("msr DAIFClr, #2");
    return 0;
}

