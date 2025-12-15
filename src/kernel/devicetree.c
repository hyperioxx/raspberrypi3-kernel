#include "devicetree.h"
#include "../drivers/serial/pl011.h"
#include "endian.h"
#include "alloc.h"
#include "device.h"

// section 5.4.1 in https://devicetree-specification.readthedocs.io/en/stable/flattened-format.html

// Tokens
#define FDT_BEGIN_NODE  0x00000001
#define FDT_END_NODE    0x00000002
#define FDT_PROP        0x00000003
#define FDT_NOP         0x00000004
#define FDT_END         0x00000009


#define DT_MAX_DEPTH 512


static  inline int streq(const char *a, const char *b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return (*a == 0 && *b == 0);
}

const struct fdt_header* parse_fdt_header(uintptr_t dtb_ptr){
    
    const struct fdt_header *hdr = (const struct fdt_header *)dtb_ptr;

    uint32_t magic = be32_to_cpu(hdr->magic);
    if (magic != 0xd00dfeed) {
        return NULL;
    }
    return hdr;
}


static inline uintptr_t align4(uintptr_t x) {
    return (x + 3u) & ~((uintptr_t)3u);
}

static uint64_t read_cells(uintptr_t p, int cells) {
    uint64_t v = 0;
    for (int i = 0; i < cells; i++) {
        v = (v << 32) | be32_to_cpu(*(uint32_t *)(p + i * 4));
    }
    return v;
}

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

static void print_device(struct device *d, int depth) {
    for (int i = 0; i < depth; i++)
        pl011_write("  ");

    pl011_write("node: ");
    pl011_write(d->name ? d->name : "(anon)");

    if (d->compatible) {
        pl011_write(" compat=");
        pl011_write(d->compatible);
    }

    if (d->mmio_size) {
        pl011_write(" reg=");
        print_hex64(d->mmio_base);
        pl011_write("+");
        print_hex64(d->mmio_size);
    }

    if (!d->enabled)
        pl011_write(" [disabled]");

    pl011_write("\n");
}

void parse_fdt(const struct fdt_header *hdr) {
    pl011_write("parse_fdt entered\n");

    uintptr_t base        = (uintptr_t)hdr;
    uintptr_t struct_blk  = base + be32_to_cpu(hdr->off_dt_struct);
    uintptr_t strings_blk = base + be32_to_cpu(hdr->off_dt_strings);
    uintptr_t struct_end  = struct_blk + be32_to_cpu(hdr->size_dt_struct);
    uintptr_t strings_end = strings_blk + be32_to_cpu(hdr->size_dt_strings);

    uintptr_t cursor = struct_blk;

    struct dt_frame {
        struct device dev;
        int addr_cells;
        int size_cells;
    };

    struct dt_frame *stack =
        (struct dt_frame *)kalloc(sizeof(struct dt_frame) * DT_MAX_DEPTH);

    if (!stack) {
        pl011_write("DT ERROR: kalloc failed\n");
        return;
    }

    int depth = 0;

    while (cursor + 4 <= struct_end) {
        uintptr_t prev_cursor = cursor;

        uint32_t token = be32_to_cpu(*(uint32_t *)cursor);
        cursor += 4;

        switch (token) {

        case FDT_BEGIN_NODE: {

            if (depth >= DT_MAX_DEPTH) {
                return;
            }

            const char *name = (const char *)cursor;
            while (cursor < struct_end && *(char *)cursor)
                cursor++;

            if (cursor >= struct_end) {
                return;
            }

            cursor++;               // skip NUL
            cursor = align4(cursor);

            struct dt_frame *f = &stack[depth];
            struct device *d   = &f->dev;

            d->name       = name;
            d->compatible = 0;
            d->mmio_base  = 0;
            d->mmio_size  = 0;
            d->enabled    = 1;

            if (depth == 0) {
                f->addr_cells = 2;
                f->size_cells = 1;
            } else {
                f->addr_cells = stack[depth - 1].addr_cells;
                f->size_cells = stack[depth - 1].size_cells;
            }

            depth++;
            break;
        }

        case FDT_END_NODE:

            depth--;
            if (depth < 0) {
                return;
            }

            struct device *d = &stack[depth].dev;

            print_device(d, depth);

            if (d->compatible && d->mmio_size && d->enabled) {
                device_register(d);
            }

            break;

        case FDT_PROP: {

            if (cursor + 8 > struct_end) {
                return;
            }

            uint32_t len     = be32_to_cpu(*(uint32_t *)cursor); cursor += 4;
            uint32_t nameoff = be32_to_cpu(*(uint32_t *)cursor); cursor += 4;

            uintptr_t value = cursor;
            cursor = align4(cursor + len);

            break;
        }

        case FDT_NOP:
            break;

        case FDT_END:
            return;

        default:
            return;
        }

        if (cursor <= prev_cursor) {
            return;
        }
    }

}

