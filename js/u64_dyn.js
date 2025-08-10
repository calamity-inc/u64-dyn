'use strict';

function pack_u64_dyn(v) {
  if (typeof v !== 'bigint') v = BigInt(v);
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
  while (offset + used < buf.length && used < 8) {
    const b = BigInt(buf[offset + used++]);
    v += (b & 0x7fn) << bits;
    if ((b & 0x80n) === 0n) {
      return [v, offset + used];
    }
    bits += 7n;
  }
  if (offset + used < buf.length) {
    const b = BigInt(buf[offset + used++]);
    v += b << 56n;
  }
  return [v, offset + used];
}

function pack_u64_dyn_v2(v) {
  if (typeof v !== 'bigint') v = BigInt(v);
  const out = [];
  for (let i = 0; i < 8; i++) {
    const cur = v & 0x7fn;
    v >>= 7n;
    if (v !== 0n) {
      out.push(Number(cur | 0x80n));
      v -= 1n;
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
  while (offset + used < buf.length && used < 8) {
    const b = BigInt(buf[offset + used++]);
    v += (b & 0x7fn) << bits;
    if ((b & 0x80n) === 0n) {
      return [v, offset + used];
    }
    bits += 7n;
    v += 1n << bits;
  }
  if (offset + used < buf.length) {
    const b = BigInt(buf[offset + used++]);
    v += b << 56n;
  }
  return [v, offset + used];
}

function pack_i64_dyn(v) {
  if (typeof v !== 'bigint') v = BigInt(v);
  const neg = v < 0n ? 1n : 0n;
  let u;
  if (neg) {
    u = (-v) & ((1n << 63n) - 1n);
  } else {
    u = v;
  }
  const packed = (neg << 6n) | ((u & ~0x3fn) << 1n) | (u & 0x3fn);
  return pack_u64_dyn(packed);
}

function unpack_i64_dyn(buf, offset = 0) {
  const [u64, idx] = unpack_u64_dyn(buf, offset);
  let u = u64;
  const neg = (u >> 6n) & 1n;
  u = ((u >> 1n) & ~0x3fn) | (u & 0x3fn);
  let v;
  if (neg) {
    if (u === 0n) {
      v = -(1n << 63n);
    } else {
      v = -u;
    }
  } else {
    v = u;
  }
  return [v, idx];
}

function pack_i64_dyn_v2(v) {
  if (typeof v !== 'bigint') v = BigInt(v);
  const neg = v < 0n ? 1n : 0n;
  let u;
  if (neg) {
    u = (-v) - 1n;
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
    v = (-u) - 1n;
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
  unpack_i64_dyn_v2
};

if (require.main === module) {
  const casesU64 = new Map([
    [0n, [0x00]],
    [0x7fn, [0x7f]],
    [0x80n, [0x80, 0x01]],
    [1337n, [0xb9, 0x0a]],
    [42069n, [0xd5, 0xc8, 0x02]],
    [0xffffffffffffffffn, [0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff]],
    [0x8000000000000000n, [0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80]]
  ]);
  for (const [val, enc] of casesU64) {
    const packed = Array.from(pack_u64_dyn(val));
    console.assert(JSON.stringify(packed) === JSON.stringify(enc), 'pack_u64_dyn mismatch', val);
    const [value, off] = unpack_u64_dyn(Uint8Array.from(enc));
    console.assert(value === val && off === enc.length, 'unpack_u64_dyn mismatch', val);
  }

  const casesI64 = new Map([
    [0n, [0x00]],
    [0x7fn, [0xbf, 0x01]],
    [0x80n, [0x80, 0x02]],
    [1337n, [0xb9, 0x14]],
    [42069n, [0x95, 0x91, 0x05]],
    [0xffffffffffffffffn, [0x41]],
    [0x8000000000000000n, [0x40]]
  ]);
  for (const [val, enc] of casesI64) {
    const packed = Array.from(pack_i64_dyn(BigInt.asIntN(64, val)));
    console.assert(JSON.stringify(packed) === JSON.stringify(enc), 'pack_i64_dyn mismatch', val);
    const [value, off] = unpack_i64_dyn(Uint8Array.from(enc));
    console.assert(value === BigInt.asIntN(64, val) && off === enc.length, 'unpack_i64_dyn mismatch', val);
  }

  const casesU64v2 = new Map([
    [0n, [0x00]],
    [0x7fn, [0x7f]],
    [0x80n, [0x80, 0x00]],
    [1337n, [0xb9, 0x09]],
    [42069n, [0xd5, 0xc7, 0x01]],
    [0xffffffffffffffffn, [0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe]],
    [0x8000000000000000n, [0x80, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x7e]]
  ]);
  for (const [val, enc] of casesU64v2) {
    const packed = Array.from(pack_u64_dyn_v2(val));
    console.assert(JSON.stringify(packed) === JSON.stringify(enc), 'pack_u64_dyn_v2 mismatch', val);
    const [value, off] = unpack_u64_dyn_v2(Uint8Array.from(enc));
    console.assert(value === val && off === enc.length, 'unpack_u64_dyn_v2 mismatch', val);
  }

  const casesI64v2 = new Map([
    [0n, [0x00]],
    [0x7fn, [0xbf, 0x00]],
    [0x80n, [0x80, 0x01]],
    [1337n, [0xb9, 0x13]],
    [42069n, [0x95, 0x90, 0x04]],
    [0xffffffffffffffffn, [0x40]],
    [0x8000000000000000n, [0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe]]
  ]);
  for (const [val, enc] of casesI64v2) {
    const packed = Array.from(pack_i64_dyn_v2(BigInt.asIntN(64, val)));
    console.assert(JSON.stringify(packed) === JSON.stringify(enc), 'pack_i64_dyn_v2 mismatch', val);
    const [value, off] = unpack_i64_dyn_v2(Uint8Array.from(enc));
    console.assert(value === BigInt.asIntN(64, val) && off === enc.length, 'unpack_i64_dyn_v2 mismatch', val);
  }
}
