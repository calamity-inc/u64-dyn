u64_dyn, i64_dyn, u64_dyn_v2, and i64_dyn_v2 are variable-length codings of 64-bit integers that require less bytes for smaller integers. Notably, encoded values never exceed 9 bytes.

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
`42` | `2a`
`0x7f` | `7f`
`0x80` | `80 01`
`0x4000` | `80 80 01`

### u64_dyn_v2

Same as u64_dyn but for each continuation bit (i.e., "another byte follows"), 1 is subtracted from the remaining value after shifting. Decoding compensates by adding a bias.

Value | Encoded As
------|-----------
`42` | `2a`
`0x7f` | `7f`
`0x80` | `80 00`
`0x4000` | `80 7f`

Note that there are contrived sequences for which `pack_u64_dyn_v2(unpack_u64_dyn_v2(seq)) == seq` does not hold, e.g. `ff ff fe fe fe fe fe fe fe`.

### i64_dyn

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
This value is then encoded using u64_dyn. When decoding, if `neg` is set the original value is recovered with:
```
i64 := ~(u63 - 1) | (1 << 63)
```

Value | Encoded As
------|-----------
`42` | `2a`
`0x2000` | `80 80 01`
`-1` | `41`
`-9223372036854775808` | `40`

### i64_dyn_v2

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
This value is then encoded using u64_dyn_v2. Decoding is the same in reverse.

Value | Encoded As
------|-----------
`42` | `2a`
`0x2000` | `80 7f`
`-1` | `40`
`-9223372036854775808` | `ff fe fe fe fe fe fe fe fe`
