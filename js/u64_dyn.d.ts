export function pack_u64_dyn(value: bigint | number): Uint8Array;
export function unpack_u64_dyn(
  data: Uint8Array,
  offset?: number,
): [bigint, number];

export function pack_u64_dyn_b(value: bigint | number): Uint8Array;
export function unpack_u64_dyn_b(
  data: Uint8Array,
  offset?: number,
): [bigint, number];

export function pack_u64_dyn_p(value: bigint | number): Uint8Array;
export function unpack_u64_dyn_p(
  data: Uint8Array,
  offset?: number,
): [bigint, number];

export function pack_u64_dyn_bp(value: bigint | number): Uint8Array;
export function unpack_u64_dyn_bp(
  data: Uint8Array,
  offset?: number,
): [bigint, number];

export function pack_i64_dyn_a(value: bigint | number): Uint8Array;
export function unpack_i64_dyn_a(
  data: Uint8Array,
  offset?: number,
): [bigint, number];

export function pack_i64_dyn_b(value: bigint | number): Uint8Array;
export function unpack_i64_dyn_b(
  data: Uint8Array,
  offset?: number,
): [bigint, number];

/** @deprecated Renamed to pack_u64_dyn_b */
export function pack_u64_dyn_v2(value: bigint | number): Uint8Array;
/** @deprecated Renamed to unpack_u64_dyn_b */
export function unpack_u64_dyn_v2(
  data: Uint8Array,
  offset?: number,
): [bigint, number];
/** @deprecated Renamed to pack_i64_dyn_a */
export function pack_i64_dyn(value: bigint | number): Uint8Array;
/** @deprecated Renamed to unpack_i64_dyn_a */
export function unpack_i64_dyn(
  data: Uint8Array,
  offset?: number,
): [bigint, number];
/** @deprecated Renamed to pack_i64_dyn_b */
export function pack_i64_dyn_v2(value: bigint | number): Uint8Array;
/** @deprecated Renamed to unpack_i64_dyn_b */
export function unpack_i64_dyn_v2(
  data: Uint8Array,
  offset?: number,
): [bigint, number];
