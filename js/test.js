const {
  pack_u64_dyn,
  unpack_u64_dyn,
  pack_u64_dyn_v2,
  unpack_u64_dyn_v2,
  pack_i64_dyn,
  unpack_i64_dyn,
  pack_i64_dyn_v2,
  unpack_i64_dyn_v2
} = require('./u64_dyn');

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
