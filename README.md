Variable-length 64-bit integer codings that take at most 9 bytes.

- Unsigned
  - [u64_dyn](#u64_dyn)
  - [u64_dyn_b](#u64_dyn_b) *(fka. `u64_dyn_v2`)*
  - [u64_dyn_p](#u64_dyn_p)
  - [u64_dyn_bp](#u64_dyn_bp)
- Signed
  - [i64_dyn_a](#i64_dyn_a) *(fka. `i64_dyn`)*
  - [i64_dyn_b](#i64_dyn_b) *(fka. `i64_dyn_v2`)*
  - [i64_dyn_bp](#i64_dyn_bp)

### Guide
- Biased variants (indicated with a `b`) need less bytes to encode certain sequences, but also leave room for multiple 9-byte sequences producing the same value (modulo 2^64).
- Prefixed variants (indicated with a `p`) are a lot faster to read due to the first byte indicating the sequence length.
- For signed codings, there is also `a` to indicate arithmetic negation.

## Implementations

- [C](c)
- C++
    - [Soup](https://github.com/calamity-inc/Soup) has implementations for [reading](https://github.com/calamity-inc/Soup/blob/senpai/soup/Reader.cpp) and [writing](https://github.com/calamity-inc/Soup/blob/senpai/soup/Writer.cpp) — these are vectorized if supported by the CPU
- [JavaScript](js)
- [Lua](lua/u64_dyn.lua)
    - The [APM](https://github.com/PlutoLang/apm#readme) package for this can be found [here](https://gist.github.com/Sainan/02c3ac9cea5015341412c92feec95e56)
- [PHP](php)
- [Rust](rust)

## Specifications

### u64_dyn

The first 56 bits of the integer are encoded in up to eight little-endian 7-bit groups where the most significant bit indicates if another byte follows. The final 8 bits are encoded as-is.

Value | Encoded As
------|-----------
`0x7f` | `7f`
`0x80` | `80 01`
`0x4000` | `80 80 01`
`0xffffffffffffffff` | `ff ff ff ff ff ff ff ff ff`

### u64_dyn_b

Same as [u64_dyn](#u64_dyn) but for each continuation bit (i.e., "another byte follows"), 1 is subtracted from the remaining value after shifting. Decoding compensates by adding a bias.

Value | Encoded As
------|-----------
`0x7f` | `7f`
`0x80` | `80 00`
`0x4000` | `80 7f`
`0xffffffffffffffff` | `ff fe fe fe fe fe fe fe fe`

Note that there are contrived sequences for which `pack_u64_dyn_b(unpack_u64_dyn_b(seq)) == seq` does not hold, e.g. `ff ff fe fe fe fe fe fe fe`.

### u64_dyn_p

Same as [u64_dyn](#u64_dyn) but the continuation bits are all moved to the first byte:
```
0xxxxxxx
10xxxxxx xxxxxxxx
110xxxxx xxxxxxxx xxxxxxxx
1110xxxx xxxxxxxx xxxxxxxx xxxxxxxx
11110xxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
111110xx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
1111110x xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
11111110 xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
11111111 xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
```
Prefixing does not change the length of coded values.

Value | Encoded As
------|-----------
`0x7f` | `7f`
`0x80` | `80 02`
`0x4000` | `C0 80 02`
`0xffffffffffffffff` | `ff ff ff ff ff ff ff ff ff`

### u64_dyn_bp

This coding uses the biasing of [u64_dyn_b](#u64_dyn_b) and the prefixing of [u64_dyn_p](#u64_dyn_p).

Value | Encoded As
------|-----------
`0x7f` | `7f`
`0x80` | `80 00`
`0x4000` | `80 fe`
`0xffffffffffffffff` | `ff 7f bf df ef f7 fb fd fe`

### i64_dyn_a

The i64 value is split into (neg, u63) like so:
```
neg := i64 < 0
if neg:
    u63 := (~i64 + 1) & ~(1 << 63)
else:
    u63 := i64
```
This is rejoined into a u64 with neg in bit 6:
```
u64 := (neg << 6) | ((u63 & ~0x3f) << 1) | (u63 & 0x3f)
```
This value is then encoded using [u64_dyn](#u64_dyn). When decoding, if `neg` is set the original value is recovered with:
```
i64 := ~(u63 - 1) | (1 << 63)
```

Value | Encoded As
------|-----------
`42` | `2a`
`0x2000` | `80 80 01`
`-1` | `41`
`-9223372036854775808` | `40`

### i64_dyn_b

The i64 value is split into (neg, u63) like so:
```
neg := i64 < 0
if neg:
    u63 := ~i64
else:
    u63 := i64
```
This is rejoined into a u64 with neg in bit 6:
```
u64 := (neg << 6) | ((u63 & ~0x3f) << 1) | (u63 & 0x3f)
```
This value is then encoded using [u64_dyn_b](#u64_dyn_b). Decoding is the same in reverse.

Value | Encoded As
------|-----------
`42` | `2a`
`0x2000` | `80 7f`
`-1` | `40`
`-9223372036854775808` | `ff fe fe fe fe fe fe fe fe`

### i64_dyn_bp

The i64 value is converted to u64 just like in [i64_dyn_b](#i64_dyn_b). However, in this case the u64 is coded with [u64_dyn_bp](#u64_dyn_bp).

Value | Encoded As
------|-----------
`42` | `2a`
`0x2000` | `80 fe`
`-1` | `40`
`-9223372036854775808` | `ff 7f bf df ef f7 fb fd fe`
