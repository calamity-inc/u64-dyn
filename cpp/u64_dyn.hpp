#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>

template <int Version>
inline size_t pack_u64_dyn(uint8_t out[9], uint64_t v)
{
    static_assert(Version == 1 || Version == 2, "Version must be 1 or 2");
    size_t i = 0;
    while (i != 8)
    {
        const uint8_t cur = v & 0x7f;
        v >>= 7;
        if (v != 0)
        {
            out[i++] = cur | 0x80;
            if constexpr (Version == 2)
            {
                v -= 1;
            }
        }
        else
        {
            out[i++] = cur;
            return i;
        }
    }
    if (v != 0)
    {
        out[i++] = static_cast<uint8_t>(v);
    }
    return i;
}

template <int Version>
inline uint64_t unpack_u64_dyn(const uint8_t* in_data, size_t in_size, size_t* out_size)
{
    static_assert(Version == 1 || Version == 2, "Version must be 1 or 2");
    if (!in_data)
    {
        throw std::runtime_error("malformed dynamic integer");
    }
    uint64_t v = 0;
    int bits = 0;
    size_t used = 0;
    while (true)
    {
        if (used >= in_size)
        {
            throw std::runtime_error("malformed dynamic integer");
        }
        uint8_t b = in_data[used++];
        if (used == 9)
        {
            v += static_cast<uint64_t>(b) << bits;
            break;
        }
        v += static_cast<uint64_t>(b & 0x7f) << bits;
        if ((b & 0x80) == 0)
        {
            break;
        }
        bits += 7;
        if constexpr (Version == 2)
        {
            v += static_cast<uint64_t>(1) << bits;
        }
    }
    if (out_size)
    {
        *out_size = used;
    }
    return v;
}

template <int Version>
inline size_t pack_i64_dyn(uint8_t out[9], int64_t v)
{
    static_assert(Version == 1 || Version == 2, "Version must be 1 or 2");
    uint64_t u = static_cast<uint64_t>(v);
    const uint64_t neg = static_cast<uint64_t>(v < 0);
    if constexpr (Version == 2)
    {
        if (neg)
        {
            u = ~u;
        }
    }
    else
    {
        if (neg)
        {
            u = (~u + 1) & ~(static_cast<uint64_t>(1) << 63);
        }
    }
    return pack_u64_dyn<Version>(out, (neg << 6) | ((u & ~0x3fULL) << 1) | (u & 0x3fULL));
}

template <int Version>
inline int64_t unpack_i64_dyn(const uint8_t* in_data, size_t in_size, size_t* out_size)
{
    static_assert(Version == 1 || Version == 2, "Version must be 1 or 2");
    uint64_t u = unpack_u64_dyn<Version>(in_data, in_size, out_size);
    const uint64_t neg = (u >> 6) & 1;
    u = ((u >> 1) & ~0x3fULL) | (u & 0x3fULL);
    int64_t v;
    if constexpr (Version == 2)
    {
        v = static_cast<int64_t>(u);
        if (neg)
        {
            v = ~v;
        }
    }
    else
    {
        v = static_cast<int64_t>(u);
        if (neg)
        {
            v = static_cast<int64_t>(~(u - 1) | (static_cast<uint64_t>(1) << 63));
        }
    }
    return v;
}

