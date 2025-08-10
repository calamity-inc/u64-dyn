#include "u64_dyn.h"
#include <errno.h>

size_t pack_u64_dyn(uint8_t out[9], uint64_t v)
{
    size_t i = 0;
    while (i != 8)
    {
        const uint8_t cur = v & 0x7f;
        v >>= 7;
        if (v != 0)
        {
            out[i++] = cur | 0x80;
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

uint64_t unpack_u64_dyn(const uint8_t* in_data, size_t in_size, size_t* out_size)
{
    if (in_size == 0)
    {
        errno = EINVAL;
        if (out_size)
        {
            *out_size = 0;
        }
        return 0;
    }
    uint64_t v = 0;
    int bits = 0;
    size_t used = 0;
    uint8_t b = 0;
    while (used < in_size && used < 8)
    {
        b = in_data[used++];
        v += (uint64_t)(b & 0x7f) << bits;
        if ((b & 0x80) == 0)
        {
            goto done;
        }
        bits += 7;
    }
    if (used < in_size)
    {
        b = in_data[used++];
        v += (uint64_t)b << 56;
    }
    else if ((b & 0x80) != 0)
    {
        errno = EINVAL;
        if (out_size)
        {
            *out_size = 0;
        }
        return 0;
    }
  done:
    if (out_size)
    {
        *out_size = used;
    }
    return v;
}

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
    if (in_size == 0)
    {
        errno = EINVAL;
        if (out_size)
        {
            *out_size = 0;
        }
        return 0;
    }
    uint64_t v = 0;
    int bits = 0;
    size_t used = 0;
    uint8_t b = 0;
    while (used < in_size && used < 8)
    {
        b = in_data[used++];
        v += (uint64_t)(b & 0x7f) << bits;
        if ((b & 0x80) == 0)
        {
            goto done;
        }
        bits += 7;
        v += (uint64_t)1 << bits; // v2
    }
    if (used < in_size)
    {
        b = in_data[used++];
        v += (uint64_t)b << 56;
    }
    else if ((b & 0x80) != 0)
    {
        errno = EINVAL;
        if (out_size)
        {
            *out_size = 0;
        }
        return 0;
    }
  done:
    if (out_size)
    {
        *out_size = used;
    }
    return v;
}

size_t pack_i64_dyn(uint8_t out[9], int64_t v)
{
    uint64_t u = (uint64_t)v;
    uint64_t neg = (uint64_t)(v < 0);
    if (neg)
    {
        u = (~u + 1) & ~((uint64_t)1 << 63);
    }
    return pack_u64_dyn(out, (neg << 6) | ((u & ~0x3f) << 1) | (u & 0x3f));
}

int64_t unpack_i64_dyn(const uint8_t* in_data, size_t in_size, size_t* out_size)
{
    uint64_t u = unpack_u64_dyn(in_data, in_size, out_size);
    const int neg = (u >> 6) & 1;
    u = ((u >> 1) & ~((uint64_t)0x3f)) | (u & 0x3f);
    if (neg)
    {
        if (u == 0)
        {
            return (int64_t)1 << 63;
        }
        return -(int64_t)u;
    }
    return (int64_t)u;
}

size_t pack_i64_dyn_v2(uint8_t out[9], int64_t v)
{
    uint64_t u;
    uint64_t neg = (v >> 63);
    if (neg)
    {
        u = ~v;
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
        v = ~u;
    }
    else
    {
        v = u;
    }
    return v;
}
