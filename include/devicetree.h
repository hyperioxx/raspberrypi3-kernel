// took the struct directly from https://devicetree-specification.readthedocs.io/en/stable/flattened-format.html
//
#ifndef DEVICETREE_H
#define DEVICETREE_H

#include "types.h"

struct fdt_header {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
}__attribute__((packed));

const struct fdt_header* parse_fdt_header(uintptr_t dtb_ptr);
#endif
