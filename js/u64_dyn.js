"use strict";

function pack_u64_dyn(v) {
  if (typeof v !== "bigint") v = BigInt(v);
  v = BigInt.asUintN(64, v);
  const out = [];
  for (let i = 0; i < 8; i++) {
    const cur = v & 0x7fn;
    v >>= 7n;
    if (v !== 0n) {
      out.push(Number(cur | 0x80n));
    } else {
      out.push(Number(cur));
      return Uint8Array.from(out);
    }
  }
  if (v !== 0n) {
    out.push(Number(v));
  }
  return Uint8Array.from(out);
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
      return [BigInt.asUintN(64, v), offset + used + 1];
    }
    bits += 7n;
  }
  if (offset + used >= buf.length) throw new RangeError("Insufficient data");
  const b = BigInt(buf[offset + used]);
  v += b << 56n;
  return [BigInt.asUintN(64, v), offset + used + 1];
}

function pack_u64_dyn_v2(v) {
  if (typeof v !== "bigint") v = BigInt(v);
  v = BigInt.asUintN(64, v);
  const out = [];
  for (let i = 0; i < 8; i++) {
    const cur = v & 0x7fn;
    v >>= 7n;
    if (v !== 0n) {
      out.push(Number(cur | 0x80n));
      v -= 1n; // v2
    } else {
      out.push(Number(cur));
      return Uint8Array.from(out);
    }
  }
  if (v !== 0n) {
    out.push(Number(v));
  }
  return Uint8Array.from(out);
}

function unpack_u64_dyn_v2(buf, offset = 0) {
  let v = 0n;
  let bits = 0n;
  let used = 0;
  for (; used < 8; used++) {
    if (offset + used >= buf.length) throw new RangeError("Insufficient data");
    const b = BigInt(buf[offset + used]);
    v += (b & 0x7fn) << bits;
    if ((b & 0x80n) === 0n) {
      return [BigInt.asUintN(64, v), offset + used + 1];
    }
    bits += 7n;
    v += 1n << bits; // v2
  }
  if (offset + used >= buf.length) throw new RangeError("Insufficient data");
  const b = BigInt(buf[offset + used]);
  v += b << 56n;
  return [BigInt.asUintN(64, v), offset + used + 1];
}

function pack_i64_dyn(v) {
  if (typeof v !== "bigint") v = BigInt(v);
  const neg = v < 0n ? 1n : 0n;
  let u = v;
  if (neg) {
    u = (~v + 1n) & ~(1n << 63n);
  }
  const packed = (neg << 6n) | ((u & ~0x3fn) << 1n) | (u & 0x3fn);
  return pack_u64_dyn(packed);
}

function unpack_i64_dyn(buf, offset = 0) {
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

function pack_i64_dyn_v2(v) {
  if (typeof v !== "bigint") v = BigInt(v);
  const neg = v < 0n ? 1n : 0n;
  let u;
  if (neg) {
    u = -v - 1n;
  } else {
    u = v;
  }
  const packed = (neg << 6n) | ((u & ~0x3fn) << 1n) | (u & 0x3fn);
  return pack_u64_dyn_v2(packed);
}

function unpack_i64_dyn_v2(buf, offset = 0) {
  const [u64, idx] = unpack_u64_dyn_v2(buf, offset);
  let u = u64;
  const neg = (u >> 6n) & 1n;
  u = ((u >> 1n) & ~0x3fn) | (u & 0x3fn);
  let v;
  if (neg) {
    v = -u - 1n;
  } else {
    v = u;
  }
  return [v, idx];
}

module.exports = {
  pack_u64_dyn,
  unpack_u64_dyn,
  pack_u64_dyn_v2,
  unpack_u64_dyn_v2,
  pack_i64_dyn,
  unpack_i64_dyn,
  pack_i64_dyn_v2,
  unpack_i64_dyn_v2,
};
