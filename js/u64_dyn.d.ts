export function pack_u64_dyn(value: bigint): Uint8Array;
export function unpack_u64_dyn(
  data: Uint8Array,
  offset?: number,
): [bigint, number];
export function pack_u64_dyn_v2(value: bigint): Uint8Array;
export function unpack_u64_dyn_v2(
  data: Uint8Array,
  offset?: number,
): [bigint, number];
export function pack_i64_dyn(value: bigint): Uint8Array;
export function unpack_i64_dyn(
  data: Uint8Array,
  offset?: number,
): [bigint, number];
export function pack_i64_dyn_v2(value: bigint): Uint8Array;
export function unpack_i64_dyn_v2(
  data: Uint8Array,
  offset?: number,
): [bigint, number];
