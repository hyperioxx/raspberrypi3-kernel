#include "devicetree.h"
#include "endian.h"
// section 5.4.1 in https://devicetree-specification.readthedocs.io/en/stable/flattened-format.html

// Tokens
#define FDT_BEGIN_NODE  0x00000001
#define FDT_END_NODE    0x00000002
#define FDT_PROP        0x00000003
#define FDT_NOP         0x00000004
#define FDT_END         0x00000009

const struct fdt_header* parse_fdt_header(uintptr_t dtb_ptr){
    
    const struct fdt_header *hdr = (const struct fdt_header *)dtb_ptr;

    uint32_t magic = be32_to_cpu(hdr->magic);
    if (magic != 0xd00dfeed) {
        return NULL;
    }
    return hdr;
}

void parse_fdt(const struct fdt_header *hdr) {
    //TODO: make parser
}
