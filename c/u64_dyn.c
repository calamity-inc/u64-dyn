#include "u64_dyn.h"

size_t pack_u64_dyn_v2(uint8_t out[9], uint64_t v)
{
    size_t i = 0;
    while (i != 8)
    {
        const uint8_t cur = v & 0x7f;
        v >>= 7;
        if (v != 0)
        {
            out[i++] = cur | 0x80;
            v -= 1; // v2
        }
        else
        {
            out[i++] = cur;
            return i;
        }
    }
    if (v != 0)
    {
        out[i++] = v;
    }
    return i;
}

uint64_t unpack_u64_dyn_v2(const uint8_t* in_data, size_t in_size, size_t* out_size)
{
    uint64_t v = 0;
    int bits = 0;
    size_t used = 0;
    while (used < in_size && used < 8)
    {
        uint8_t b = in_data[used++];
        v += (uint64_t)(b & 0x7f) << bits;
        if ((b & 0x80) == 0)
        {
            goto done;
        }
        bits += 7;
        v += (uint64_t)1 << bits;
    }
    if (used < in_size)
    {
        uint8_t b = in_data[used++];
        v += (uint64_t)b << 56;
    }
  done:
    if (out_size)
    {
        *out_size = used;
    }
    return v;
}

size_t pack_i64_dyn_v2(uint8_t out[9], int64_t v)
{
    uint64_t u;
    uint64_t neg = (v >> 63);
    if (neg)
    {
        u = (v * -1) - 1;
    }
    else
    {
        u = v;
    }
    return pack_u64_dyn_v2(out, (neg << 6) | ((u & ~0x3f) << 1) | (u & 0x3f));
}

int64_t unpack_i64_dyn_v2(const uint8_t* in_data, size_t in_size, size_t* out_size)
{
    uint64_t u = unpack_u64_dyn_v2(in_data, in_size, out_size);
    const int neg = (u >> 6) & 1; // check bit 6
    u = ((u >> 1) & ~0x3f) | (u & 0x3f); // remove bit 6
    int64_t v;
    if (neg)
    {
        v = (u * -1) - 1;
    }
    else
    {
        v = u;
    }
    return v;
}
