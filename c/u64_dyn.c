#include "u64_dyn.h"

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

size_t pack_u64_dyn_v2(uint8_t out[9], uint64_t v) {
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

bool unpack_u64_dyn_v2(const uint8_t *in_data, size_t in_size, uint64_t *out_v,
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

size_t pack_i64_dyn(uint8_t out[9], int64_t v) {
  uint64_t u = (uint64_t)v;
  uint64_t neg = (uint64_t)(v < 0);
  if (neg) {
    u = (~u + 1) & ~((uint64_t)1 << 63);
  }
  return pack_u64_dyn(out, (neg << 6) | ((u & ~0x3f) << 1) | (u & 0x3f));
}

bool unpack_i64_dyn(const uint8_t *in_data, size_t in_size, int64_t *out_v,
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

size_t pack_i64_dyn_v2(uint8_t out[9], int64_t v) {
  uint64_t u = (uint64_t)v;
  uint64_t neg = (uint64_t)(v < 0);
  if (neg) {
    u = ~u;
  }
  return pack_u64_dyn_v2(out,
                         (neg << 6) | ((u & ~0x3fULL) << 1) | (u & 0x3fULL));
}

bool unpack_i64_dyn_v2(const uint8_t *in_data, size_t in_size, int64_t *out_v,
                       size_t *out_size) {
  uint64_t u;
  if (!unpack_u64_dyn_v2(in_data, in_size, &u, out_size)) {
    return false;
  }
  const uint64_t neg = (u >> 6) & 1;         // check bit 6
  u = ((u >> 1) & ~0x3fULL) | (u & 0x3fULL); // remove bit 6
  int64_t v = (int64_t)u;
  if (neg) {
    v = ~v;
  }
  if (out_v) {
    *out_v = v;
  }
  return true;
}
