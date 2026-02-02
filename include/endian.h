#include "types.h"

static inline uint32_t be32_to_cpu(uint32_t x)
{
    return __builtin_bswap32(x);
}
