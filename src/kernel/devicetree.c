#include "devicetree.h"


const struct fdt_header* parse_fdt_header(uintptr_t dtb_ptr){
    const struct fdt_header *hdr = (const struct fdt_header *)dtb_ptr;
    return hdr;
}

void parse_fdt(const struct fdt_header *hdr) {
    //TODO: make parser
}
