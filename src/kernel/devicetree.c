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


struct range_entry {
        uint64_t child;
        uint64_t parent;
        uint64_t size;
    };

#define MAX_RANGES 8

struct dt_frame {
        struct device dev;
        int addr_cells;
        int size_cells;
        int range_count;
        struct range_entry ranges[MAX_RANGES];
};


static  inline int streq(const char *a, const char *b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return (*a == 0 && *b == 0);
}

static inline size_t str_len(const char *s) {
    size_t n = 0;
    if (!s) return 0;
    while (*s++) n++;
    return n;
}

static inline int starts_with(const char *s, const char *prefix) {
    while (*prefix) {
        if (*s++ != *prefix++)
            return 0;
    }
    return 1;
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

    if (d->compatible && d->compatible_len) {
        pl011_write(" compat=[");
        const char *p = d->compatible;
        const char *end = p + d->compatible_len;

        while (p < end && *p) {
            pl011_write(p);
            p += str_len(p) + 1;
            if (p < end && *p)
                pl011_write(", ");
        }
        pl011_write("]");
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


static uint64_t translate_addr(struct dt_frame *parent_bus, uint64_t child_addr) {
    // If no ranges, identity mapping.
    if (!parent_bus || parent_bus->range_count == 0) return child_addr;

    for (int i = 0; i < parent_bus->range_count; i++) {
        uint64_t c = parent_bus->ranges[i].child;
        uint64_t p = parent_bus->ranges[i].parent;
        uint64_t s = parent_bus->ranges[i].size;

        if (child_addr >= c && child_addr < c + s) {
            return p + (child_addr - c);
        }
    }
    // Not found: return original (or 0 to flag error)
    return child_addr;
}


void parse_fdt(const struct fdt_header *hdr) {
    pl011_write("parse_fdt entered\n");

    uintptr_t base        = (uintptr_t)hdr;
    uintptr_t struct_blk  = base + be32_to_cpu(hdr->off_dt_struct);
    uintptr_t strings_blk = base + be32_to_cpu(hdr->off_dt_strings);
    uintptr_t struct_end  = struct_blk + be32_to_cpu(hdr->size_dt_struct);
    uintptr_t strings_end = strings_blk + be32_to_cpu(hdr->size_dt_strings);

    uintptr_t cursor = struct_blk;
    

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
            d->compatible_len = 0;
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
            f->range_count = 0;
            depth++;
            break;
        }

        case FDT_END_NODE:

            
            depth--;
            if (depth < 0) {
                return;
            }
            struct device *d = &stack[depth].dev;
            if (d->name && starts_with(d->name, "memory@")) {
                 break;
            }

            if (d->compatible && d->mmio_size && d->enabled) {
                print_device(d, depth); //TODO: will have to remove this at some point
                device_register(d);
            } 

            break;

        case FDT_PROP: {

            if (cursor + 8 > struct_end) {
                return;
            }
            
            struct dt_frame *f = &stack[depth - 1];
            struct device *d = &stack[depth - 1].dev;


            uint32_t len     = be32_to_cpu(*(uint32_t *)cursor); cursor += 4;
            uint32_t nameoff = be32_to_cpu(*(uint32_t *)cursor); cursor += 4;

            uintptr_t prop_value = cursor;
            cursor = align4(cursor + len);
            char *prop_name = (char *)(strings_blk + nameoff);

            if(streq(prop_name, "compatible")) {
                d->compatible = (char *) prop_value;
                d->compatible_len = len;
                break;
            }

            if (streq(prop_name, "#address-cells") && len == 4) {
                f->addr_cells = (int)be32_to_cpu(*(uint32_t*)prop_value);
                break;
            }
            if (streq(prop_name, "#size-cells") && len == 4) {
                f->size_cells = (int)be32_to_cpu(*(uint32_t*)prop_value);
                break;
            }
            if (streq(prop_name, "status")) {
                const char *s = (const char *)prop_value;
                if (s && s[0] && !streq(s, "okay")) {   // "disabled", "fail", etc.
                    f->dev.enabled = 0;
                }
                break;
            }

            if (streq(prop_name, "ranges")) {
                int child_ac = f->addr_cells;
                int child_sc = f->size_cells;
                int parent_ac = (depth >= 2) ? stack[depth - 2].addr_cells : 2;

                int stride_cells = child_ac + parent_ac + child_sc;
                int count = (stride_cells ? (len / (stride_cells * 4)) : 0);
                if (count > MAX_RANGES) count = MAX_RANGES;

                uintptr_t p = prop_value;
                f->range_count = 0;

                for (int i = 0; i < count; i++) {
                    uint64_t child  = read_cells(p, child_ac);  p += child_ac * 4;
                    uint64_t parent = read_cells(p, parent_ac); p += parent_ac * 4;
                    uint64_t size   = read_cells(p, child_sc);  p += child_sc * 4;

                    f->ranges[f->range_count++] = (struct range_entry){ child, parent, size };
                }
                break;
            }

           if (streq(prop_name, "reg")) {
                struct dt_frame *parent =
                    (depth >= 2) ? &stack[depth - 2] : NULL;

                int ac = parent ? parent->addr_cells : f->addr_cells;
                int sc = parent ? parent->size_cells : f->size_cells;

                if (len < (uint32_t)((ac + sc) * 4))
                    break;  // malformed, ignore safely

                uint64_t bus_addr = read_cells(prop_value, ac);
                uint64_t size     = read_cells(prop_value + ac * 4, sc);

                uint64_t phys = parent
                    ? translate_addr(parent, bus_addr)
                    : bus_addr;

                f->dev.mmio_base = phys;
                f->dev.mmio_size = size;

                break;
            }
 
           /* pl011_write("UNKOWN PROP: ");
            pl011_write(prop_name);
            pl011_write(" ");
            print_hex64(prop_value);
            pl011_write("\n");
*/
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

