#include "types.h"

uint64_t main_mem_base;
uint64_t main_mem_size;

void add_main_memory(uint64_t start, uint64_t size) {
    main_mem_base = start;
    main_mem_size = size;
}

uint64_t get_main_memory_base() {

    return main_mem_size;
}
