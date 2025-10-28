#include "u64_dyn.h"

#if defined(_MSC_VER)
#include <intrin.h>
#endif

size_t pack_u64_dyn(uint8_t out[9], uint64_t v) {
  size_t i = 0;
  while (i != 8) {
    const uint8_t cur = v & 0x7f;
    v >>= 7;
    if (v != 0) {
      out[i++] = cur | 0x80;
    } else {
      out[i++] = cur;
      return i;
    }
  }
  out[i++] = v;
  return i;
}

bool unpack_u64_dyn(const uint8_t *in_data, size_t in_size, uint64_t *out_v,
                    size_t *out_size) {
  uint64_t v = 0;
  int bits = 0;
  size_t used = 0;
  while (1) {
    if (used >= in_size) {
      return false;
    }
    uint8_t b = in_data[used++];
    if (used == 9) {
      v += (uint64_t)b << bits;
      break;
    }
    v += (uint64_t)(b & 0x7f) << bits;
    if ((b & 0x80) == 0) {
      break;
    }
    bits += 7;
  }
  if (out_size) {
    *out_size = used;
  }
  if (out_v) {
    *out_v = v;
  }
  return true;
}

size_t pack_u64_dyn_b(uint8_t out[9], uint64_t v) {
  size_t i = 0;
  while (i != 8) {
    const uint8_t cur = v & 0x7f;
    v >>= 7;
    if (v != 0) {
      out[i++] = cur | 0x80;
      v -= 1; // v2
    } else {
      out[i++] = cur;
      return i;
    }
  }
  out[i++] = v;
  return i;
}

bool unpack_u64_dyn_b(const uint8_t *in_data, size_t in_size, uint64_t *out_v,
                      size_t *out_size) {
  uint64_t v = 0;
  int bits = 0;
  size_t used = 0;
  while (1) {
    if (used >= in_size) {
      return false;
    }
    uint8_t b = in_data[used++];
    if (used == 9) {
      v += (uint64_t)b << bits;
      break;
    }
    v += (uint64_t)(b & 0x7f) << bits;
    if ((b & 0x80) == 0) {
      break;
    }
    bits += 7;
    v += (uint64_t)1 << bits; // v2
  }
  if (out_size) {
    *out_size = used;
  }
  if (out_v) {
    *out_v = v;
  }
  return true;
}

size_t pack_i64_dyn_a(uint8_t out[9], int64_t v) {
  uint64_t u = (uint64_t)v;
  uint64_t neg = (uint64_t)(v < 0);
  if (neg) {
    u = (~u + 1) & ~((uint64_t)1 << 63);
  }
  return pack_u64_dyn(out, (neg << 6) | ((u & ~0x3f) << 1) | (u & 0x3f));
}

bool unpack_i64_dyn_a(const uint8_t *in_data, size_t in_size, int64_t *out_v,
                      size_t *out_size) {
  uint64_t u;
  if (!unpack_u64_dyn(in_data, in_size, &u, out_size)) {
    return false;
  }
  const bool neg = (u >> 6) & 1;
  int64_t v = ((u >> 1) & ~((uint64_t)0x3f)) | (u & 0x3f);
  if (neg) {
    v = (int64_t)(~(v - 1) | ((uint64_t)1 << 63));
  }
  if (out_v) {
    *out_v = v;
  }
  return true;
}

size_t pack_i64_dyn_b(uint8_t out[9], int64_t v) {
  uint64_t neg = (uint64_t)(v < 0);
  uint64_t u = v ^ (0xffffffffffffffff * neg);
  return pack_u64_dyn_b(out,
                        (neg << 6) | ((u & ~0x3fULL) << 1) | (u & 0x3fULL));
}

bool unpack_i64_dyn_b(const uint8_t *in_data, size_t in_size, int64_t *out_v,
                      size_t *out_size) {
  uint64_t u;
  if (!unpack_u64_dyn_b(in_data, in_size, &u, out_size)) {
    return false;
  }
  const uint64_t neg = (u >> 6) & 1;         // check bit 6
  u = ((u >> 1) & ~0x3fULL) | (u & 0x3fULL); // remove bit 6
  int64_t v = u ^ (0xffffffffffffffff * neg);
  if (out_v) {
    *out_v = v;
  }
  return true;
}

size_t pack_u64_dyn_p(uint8_t out[9], uint64_t v) {
  const size_t byte_length = 1 + (v >= 1ull << 7) + (v >= 1ull << 14) +
                             (v >= 1ull << 21) + (v >= 1ull << 28) +
                             (v >= 1ull << 35) + (v >= 1ull << 42) +
                             (v >= 1ull << 49) + (v >= 1ull << 56);

  const size_t first_byte_value_bits = (byte_length < 8) * (8 - byte_length);
  const size_t first_byte_prefix_bits = byte_length - 1;

  out[0] = (0xff << (8 - first_byte_prefix_bits)) |
           (v & ((1 << first_byte_value_bits) - 1));
  v >>= first_byte_value_bits;
  out[1] = v & 0xff;
  out[2] = (v >> 8) & 0xff;
  out[3] = (v >> 16) & 0xff;
  out[4] = (v >> 24) & 0xff;
  out[5] = (v >> 32) & 0xff;
  out[6] = (v >> 40) & 0xff;
  out[7] = (v >> 48) & 0xff;
  out[8] = (v >> 56) & 0xff;
  return byte_length;
}

static unsigned int getNumLeadingZeros(uint32_t mask) {
#if defined(_MSC_VER)
  unsigned long ret;
  if (_BitScanReverse(&ret, mask)) {
    return 31 - ret;
  }
#else
  if (mask != 0) {
    return __builtin_clz(mask);
  }
#endif
  return 32;
}

bool unpack_u64_dyn_p(const uint8_t *in_data, size_t in_size, uint64_t *out_v,
                      size_t *out_size) {
  if (!in_size) {
    return false;
  }
  const size_t byte_length =
      1 + (getNumLeadingZeros((uint32_t)((uint8_t)~in_data[0])) - 24);
  const size_t first_byte_value_bits = (byte_length < 8) * (8 - byte_length);
  if (byte_length > in_size) {
    return false;
  }
  uint64_t v = 0;
  for (size_t i = 1; i != byte_length; ++i) {
    v |= (uint64_t)in_data[i] << ((i - 1) * 8);
  }
  v <<= first_byte_value_bits;
  v |= (in_data[0] & ((1 << first_byte_value_bits) - 1));
  if (out_v) {
    *out_v = v;
  }
  if (out_size) {
    *out_size = byte_length;
  }
  return true;
}

size_t pack_u64_dyn_v2(uint8_t out[9], uint64_t v) {
  return pack_u64_dyn_b(out, v);
}

bool unpack_u64_dyn_v2(const uint8_t *in_data, size_t in_size, uint64_t *out_v,
                       size_t *out_size) {
  return unpack_u64_dyn_b(in_data, in_size, out_v, out_size);
}

size_t pack_i64_dyn(uint8_t out[9], int64_t v) {
  return pack_i64_dyn_a(out, v);
}

bool unpack_i64_dyn(const uint8_t *in_data, size_t in_size, int64_t *out_v,
                    size_t *out_size) {
  return unpack_i64_dyn_a(in_data, in_size, out_v, out_size);
}

size_t pack_i64_dyn_v2(uint8_t out[9], int64_t v) {
  return pack_i64_dyn_b(out, v);
}

bool unpack_i64_dyn_v2(const uint8_t *in_data, size_t in_size, int64_t *out_v,
                       size_t *out_size) {
  return unpack_i64_dyn_b(in_data, in_size, out_v, out_size);
}
