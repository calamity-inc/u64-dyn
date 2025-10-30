"use strict";

function get_bias(byte_length) {
  let bias = 0n;
  while (--byte_length) {
    bias += 1n;
    bias <<= 7n;
  }
  return bias;
}

function pack_u64_dyn(v) {
  if (typeof v !== "bigint") v = BigInt(v);
  const out = new Uint8Array(9);
  let length = 0;
  for (; length < 8; length++) {
    const cur = v & 0x7fn;
    v >>= 7n;
    if (v !== 0n) {
      out[length] = Number(cur | 0x80n);
    } else {
      out[length] = Number(cur);
      return out.subarray(0, length + 1);
    }
  }
  out[length++] = Number(v);
  return out.subarray(0, length);
}

function unpack_u64_dyn(buf, offset = 0) {
  let v = 0n;
  let bits = 0n;
  let used = 0;
  for (; used < 8; used++) {
    if (offset + used >= buf.length) throw new RangeError("Insufficient data");
    const b = BigInt(buf[offset + used]);
    v += (b & 0x7fn) << bits;
    if ((b & 0x80n) === 0n) {
      return [v, offset + used + 1];
    }
    bits += 7n;
  }
  if (offset + used >= buf.length) throw new RangeError("Insufficient data");
  const b = BigInt(buf[offset + used]);
  v += b << 56n;
  return [v, offset + used + 1];
}

function pack_u64_dyn_b(v) {
  if (typeof v !== "bigint") v = BigInt(v);
  const out = new Uint8Array(9);
  let length = 0;
  for (; length < 8; length++) {
    const cur = v & 0x7fn;
    v >>= 7n;
    if (v !== 0n) {
      out[length] = Number(cur | 0x80n);
      v -= 1n; // bias
    } else {
      out[length] = Number(cur);
      return out.subarray(0, length + 1);
    }
  }
  out[length++] = Number(v);
  return out.subarray(0, length);
}

function unpack_u64_dyn_b(buf, offset = 0) {
  let v = 0n;
  let bits = 0n;
  let used = 0;
  let bias = 0n;
  while (true) {
    if (offset + used >= buf.length) {
      throw new RangeError("Insufficient data");
    }
    const b = BigInt(buf[offset + used++]);
    if (used == 9) {
      v += b << 56n;
      break;
    }
    v += (b & 0x7fn) << bits;
    if ((b & 0x80n) === 0n) {
      break;
    }
    bits += 7n;
    bias += 1n << bits;
  }
  if (v > 0xffffffffffffffffn - bias) {
    throw new RangeError("Invalid data");
  }
  v += bias;
  return [v, offset + used];
}

function pack_u64_dyn_p(v) {
  if (typeof v !== "bigint") v = BigInt(v);
  const byte_length =
    1 +
    (v >= 128n) +
    (v >= 16384n) +
    (v >= 2097152n) +
    (v >= 268435456n) +
    (v >= 34359738368n) +
    (v >= 4398046511104n) +
    (v >= 562949953421312n) +
    (v >= 72057594037927936n);
  const first_byte_value_bits = (byte_length < 8) * (8 - byte_length);
  const first_byte_prefix_bits = byte_length - 1;
  const out = new Uint8Array(byte_length);
  out[0] =
    (0xff << (8 - first_byte_prefix_bits)) |
    (Number(v & 0xffn) & ((1 << first_byte_value_bits) - 1));
  v >>= BigInt(first_byte_value_bits);
  for (let i = 1; i != byte_length; ++i) {
    out[i] = Number((v >> BigInt((i - 1) * 8)) & 0xffn);
  }
  return out;
}

function unpack_u64_dyn_p(buf, offset = 0) {
  if (offset >= buf.length) {
    throw new RangeError("Insufficient data");
  }
  const byte_length = 1 + (Math.clz32(~buf[offset] & 0xff) - 24);
  const first_byte_value_bits = (byte_length < 8) * (8 - byte_length);
  if (offset + byte_length > buf.length) {
    throw new RangeError("Insufficient data");
  }
  let v = 0n;
  for (let i = 1; i != byte_length; ++i) {
    v |= BigInt(buf[offset + i]) << BigInt((i - 1) * 8);
  }
  v <<= BigInt(first_byte_value_bits);
  v |= BigInt(buf[offset] & ((1 << first_byte_value_bits) - 1));
  return [v, offset + byte_length];
}

function pack_u64_dyn_bp(v) {
  if (typeof v !== "bigint") v = BigInt(v);
  const byte_length =
    1 +
    (v >= 128n) +
    (v >= 16512n) +
    (v >= 2113664n) +
    (v >= 270549120n) +
    (v >= 34630287488n) +
    (v >= 4432676798592n) +
    (v >= 567382630219904n) +
    (v >= 72624976668147840n);
  const first_byte_value_bits = (byte_length < 8) * (8 - byte_length);
  const first_byte_prefix_bits = byte_length - 1;

  v -= get_bias(byte_length);

  const out = new Uint8Array(byte_length);
  out[0] =
    (0xff << (8 - first_byte_prefix_bits)) |
    (Number(v & 0xffn) & ((1 << first_byte_value_bits) - 1));
  v >>= BigInt(first_byte_value_bits);
  for (let i = 1; i != byte_length; ++i) {
    out[i] = Number((v >> BigInt((i - 1) * 8)) & 0xffn);
  }
  return out;
}

function unpack_u64_dyn_bp(buf, offset = 0) {
  if (offset >= buf.length) {
    throw new RangeError("Insufficient data");
  }
  const byte_length = 1 + (Math.clz32(~buf[offset] & 0xff) - 24);
  const first_byte_value_bits = (byte_length < 8) * (8 - byte_length);
  if (offset + byte_length > buf.length) {
    throw new RangeError("Insufficient data");
  }
  let v = 0n;
  for (let i = 1; i != byte_length; ++i) {
    v |= BigInt(buf[offset + i]) << BigInt((i - 1) * 8);
  }
  v <<= BigInt(first_byte_value_bits);
  v |= BigInt(buf[offset] & ((1 << first_byte_value_bits) - 1));

  const bias = get_bias(byte_length);
  if (v > 0xffffffffffffffffn - bias) {
    throw new RangeError("Invalid data");
  }
  return [v + bias, offset + byte_length];
}

function pack_i64_dyn_a(v) {
  if (typeof v !== "bigint") v = BigInt(v);
  const neg = v < 0n ? 1n : 0n;
  let u = v;
  if (neg) {
    u = (~v + 1n) & ~(1n << 63n);
  }
  const packed = (neg << 6n) | ((u & ~0x3fn) << 1n) | (u & 0x3fn);
  return pack_u64_dyn(packed);
}

function unpack_i64_dyn_a(buf, offset = 0) {
  const [u64, idx] = unpack_u64_dyn(buf, offset);
  let u = u64;
  const neg = (u >> 6n) & 1n;
  u = ((u >> 1n) & ~0x3fn) | (u & 0x3fn);
  let v = u;
  if (neg) {
    v = ~(u - 1n) | (1n << 63n);
    v = BigInt.asIntN(64, v);
  }
  return [v, idx];
}

function pack_i64_dyn_b(v) {
  if (typeof v !== "bigint") v = BigInt(v);
  const neg = v < 0n ? 1n : 0n;
  let u;
  if (neg) {
    u = ~v;
  } else {
    u = v;
  }
  const packed = (neg << 6n) | ((u & ~0x3fn) << 1n) | (u & 0x3fn);
  return pack_u64_dyn_b(packed);
}

function unpack_i64_dyn_b(buf, offset = 0) {
  const [u64, idx] = unpack_u64_dyn_b(buf, offset);
  let u = u64;
  const neg = (u >> 6n) & 1n;
  u = ((u >> 1n) & ~0x3fn) | (u & 0x3fn);
  let v;
  if (neg) {
    v = ~u;
  } else {
    v = u;
  }
  return [v, idx];
}

function pack_i64_dyn_bp(v) {
  if (typeof v !== "bigint") v = BigInt(v);
  const neg = v < 0n ? 1n : 0n;
  let u;
  if (neg) {
    u = ~v;
  } else {
    u = v;
  }
  const packed = (neg << 6n) | ((u & ~0x3fn) << 1n) | (u & 0x3fn);
  return pack_u64_dyn_bp(packed);
}

function unpack_i64_dyn_bp(buf, offset = 0) {
  const [u64, idx] = unpack_u64_dyn_bp(buf, offset);
  let u = u64;
  const neg = (u >> 6n) & 1n;
  u = ((u >> 1n) & ~0x3fn) | (u & 0x3fn);
  let v;
  if (neg) {
    v = ~u;
  } else {
    v = u;
  }
  return [v, idx];
}

function pack_u64_dyn_v2(v) {
  return pack_u64_dyn_b(v);
}

function unpack_u64_dyn_v2(buf, offset = 0) {
  return unpack_u64_dyn_b(buf, offset);
}

function pack_i64_dyn(v) {
  return pack_i64_dyn_a(v);
}

function unpack_i64_dyn(buf, offset = 0) {
  return unpack_i64_dyn_a(buf, offset);
}

function pack_i64_dyn_v2(v) {
  return pack_i64_dyn_b(v);
}

function unpack_i64_dyn_v2(buf, offset = 0) {
  return unpack_i64_dyn_b(buf, offset);
}

module.exports = {
  pack_u64_dyn,
  unpack_u64_dyn,
  pack_u64_dyn_b,
  unpack_u64_dyn_b,
  pack_u64_dyn_p,
  unpack_u64_dyn_p,
  pack_u64_dyn_bp,
  unpack_u64_dyn_bp,
  pack_i64_dyn_a,
  unpack_i64_dyn_a,
  pack_i64_dyn_b,
  unpack_i64_dyn_b,
  pack_i64_dyn_bp,
  unpack_i64_dyn_bp,
  pack_u64_dyn_v2,
  unpack_u64_dyn_v2,
  pack_i64_dyn,
  unpack_i64_dyn,
  pack_i64_dyn_v2,
  unpack_i64_dyn_v2,
};
