#ifndef IRQ_ROUTER_H
#define IRQ_ROUTER_H
#include "types.h"
struct trap_frame {
    uint64_t x[31];

    uint64_t padding;
    uint64_t elr_el1;
    uint64_t spsr_el1;
};
_Static_assert(sizeof(struct trap_frame) == 272, "trap_frame size mismatch");
void handle_interrupt_c(struct trap_frame *tf);
#endif
