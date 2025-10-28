const assert = require("node:assert");
const {
  pack_u64_dyn,
  unpack_u64_dyn,
  pack_u64_dyn_b,
  unpack_u64_dyn_b,
  pack_i64_dyn_a,
  unpack_i64_dyn_a,
  pack_i64_dyn_b,
  unpack_i64_dyn_b,
  pack_u64_dyn_v2,
  unpack_u64_dyn_v2,
  pack_i64_dyn,
  unpack_i64_dyn,
  pack_i64_dyn_v2,
  unpack_i64_dyn_v2,
} = require("./u64_dyn");

const casesU64 = new Map([
  [0n, [0x00]],
  [0x7fn, [0x7f]],
  [0x80n, [0x80, 0x01]],
  [1337n, [0xb9, 0x0a]],
  [42069n, [0xd5, 0xc8, 0x02]],
  [0xffffffffffffffffn, [0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff]],
  [0x8000000000000000n, [0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80]],
]);
for (const [val, enc] of casesU64) {
  const packed = Array.from(pack_u64_dyn(val));
  assert.deepStrictEqual(packed, enc, `pack_u64_dyn mismatch for ${val}`);
  const [value, off] = unpack_u64_dyn(Uint8Array.from(enc));
  assert.deepStrictEqual(
    [value, off],
    [val, enc.length],
    `unpack_u64_dyn mismatch for ${val}`,
  );
}

const casesI64 = new Map([
  [0n, [0x00]],
  [0x7fn, [0xbf, 0x01]],
  [0x80n, [0x80, 0x02]],
  [1337n, [0xb9, 0x14]],
  [42069n, [0x95, 0x91, 0x05]],
  [-1n, [0x41]],
  [-9223372036854775808n, [0x40]],
]);
for (const [val, enc] of casesI64) {
  const packed = Array.from(pack_i64_dyn_a(BigInt.asIntN(64, val)));
  assert.deepStrictEqual(packed, enc, `pack_i64_dyn_a mismatch for ${val}`);
  const [value, off] = unpack_i64_dyn_a(Uint8Array.from(enc));
  assert.deepStrictEqual(
    [value, off],
    [BigInt.asIntN(64, val), enc.length],
    `unpack_i64_dyn_a mismatch for ${val}`,
  );
}

const casesU64v2 = new Map([
  [0n, [0x00]],
  [0x7fn, [0x7f]],
  [0x80n, [0x80, 0x00]],
  [1337n, [0xb9, 0x09]],
  [42069n, [0xd5, 0xc7, 0x01]],
  [0x123456789abcdefn, [0xef, 0x9a, 0xae, 0xcc, 0xf7, 0xab, 0xd0, 0x90, 0x00]],
  [0xffffffffffffffffn, [0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe]],
  [0x8000000000000000n, [0x80, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x7e]],
]);
for (const [val, enc] of casesU64v2) {
  const packed = Array.from(pack_u64_dyn_b(val));
  assert.deepStrictEqual(packed, enc, `pack_u64_dyn_b mismatch for ${val}`);
  const [value, off] = unpack_u64_dyn_b(Uint8Array.from(enc));
  assert.deepStrictEqual(
    [value, off],
    [val, enc.length],
    `unpack_u64_dyn_b mismatch for ${val}`,
  );
}

const casesI64v2 = new Map([
  [0n, [0x00]],
  [0x7fn, [0xbf, 0x00]],
  [0x80n, [0x80, 0x01]],
  [1337n, [0xb9, 0x13]],
  [42069n, [0x95, 0x90, 0x04]],
  [-1n, [0x40]],
  [
    -9223372036854775808n,
    [0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe],
  ],
]);
for (const [val, enc] of casesI64v2) {
  const packed = Array.from(pack_i64_dyn_b(BigInt.asIntN(64, val)));
  assert.deepStrictEqual(packed, enc, `pack_i64_dyn_b mismatch for ${val}`);
  const [value, off] = unpack_i64_dyn_b(Uint8Array.from(enc));
  assert.deepStrictEqual(
    [value, off],
    [BigInt.asIntN(64, val), enc.length],
    `unpack_i64_dyn_b mismatch for ${val}`,
  );
}

function expectThrow(fn, msg) {
  let threw = false;
  try {
    fn();
  } catch (e) {
    threw = true;
  }
  console.assert(threw, msg);
}

const truncatedCases = [
  [],
  [0x80],
  [0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80],
];

for (const enc of truncatedCases) {
  const buf = Uint8Array.from(enc);
  expectThrow(
    () => unpack_u64_dyn(buf),
    "unpack_u64_dyn should throw on insufficient data",
  );
  expectThrow(
    () => unpack_i64_dyn_a(buf),
    "unpack_i64_dyn_a should throw on insufficient data",
  );
  expectThrow(
    () => unpack_i64_dyn_b(buf),
    "unpack_i64_dyn_b should throw on insufficient data",
  );
  expectThrow(
    () => unpack_u64_dyn_b(buf),
    "unpack_u64_dyn_b should throw on insufficient data",
  );
}

{
  const enc = Uint8Array.from([
    0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe,
  ]);
  assert.deepStrictEqual(
    unpack_u64_dyn_b(enc),
    [0x7fn, 9],
    "unpack_u64_dyn_b should wrap modulo 2^64",
  );
  assert.deepStrictEqual(
    unpack_i64_dyn_b(enc),
    [-64n, 9],
    "unpack_i64_dyn_b should wrap modulo 2^64",
  );
}

{
  const canonical = Array.from(pack_u64_dyn_b(42069n));
  const alias = Array.from(pack_u64_dyn_v2(42069n));
  assert.deepStrictEqual(alias, canonical, "pack_u64_dyn_v2 should alias _b");
  const [value, off] = unpack_u64_dyn_v2(Uint8Array.from(alias));
  assert.deepStrictEqual(
    [value, off],
    [42069n, canonical.length],
    "unpack_u64_dyn_v2 should alias _b",
  );
}

{
  const canonical = Array.from(pack_i64_dyn_a(-123n));
  const alias = Array.from(pack_i64_dyn(-123n));
  assert.deepStrictEqual(alias, canonical, "pack_i64_dyn should alias _a");
  const [value, off] = unpack_i64_dyn(Uint8Array.from(alias));
  assert.deepStrictEqual(
    [value, off],
    [-123n, canonical.length],
    "unpack_i64_dyn should alias _a",
  );
}

{
  const canonical = Array.from(pack_i64_dyn_b(-456n));
  const alias = Array.from(pack_i64_dyn_v2(-456n));
  assert.deepStrictEqual(alias, canonical, "pack_i64_dyn_v2 should alias _b");
  const [value, off] = unpack_i64_dyn_v2(Uint8Array.from(alias));
  assert.deepStrictEqual(
    [value, off],
    [-456n, canonical.length],
    "unpack_i64_dyn_v2 should alias _b",
  );
}
