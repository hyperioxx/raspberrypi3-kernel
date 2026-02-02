#include "types.h"

extern uint8_t _heap_start;
extern uint8_t _heap_end;

static uint8_t *heap_ptr = &_heap_start;


void *kalloc(size_t size) {
    size = (size + 15) & ~((size_t)15);

    if (heap_ptr + size > &_heap_end) {
        return NULL;
    }

    void *p = heap_ptr;
    heap_ptr += size;
    return p;
}
