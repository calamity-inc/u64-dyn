u64_dyn, i64_dyn, u64_dyn_v2, and i64_dyn_v2 are variable-length codings of 64-bit integers that require less bytes for smaller integers. Notably, encoded values never exceed 9 bytes.

## Implementations

- [C](c)
- C++
    - [Soup](https://github.com/calamity-inc/Soup) has implementations for [reading](https://github.com/calamity-inc/Soup/blob/senpai/soup/Reader.cpp) and [writing](https://github.com/calamity-inc/Soup/blob/senpai/soup/Writer.cpp)
- [Lua](lua/u64_dyn.lua)
    - The [APM](https://github.com/PlutoLang/apm#readme) package for this can be found [here](https://gist.github.com/Sainan/02c3ac9cea5015341412c92feec95e56)
- [PHP](php/u64_dyn.php)
- [JavaScript](js/u64_dyn.js)

## Specifications

### u64_dyn

The first 56 bits of the integer are encoded in up to eight little-endian 7-bit groups where the most significant bit indicates if another byte follows. The final 8 bits are encoded as-is.

### u64_dyn_v2

Same as u64_dyn but for each continuation bit (i.e., "another byte follows"), 1 is subtracted from the remaining value after shifting. Decoding compensates by adding a bias.

### i64_dyn

The i64 value is split into (neg, u63) like so:
```
neg := i64 < 0
if neg:
    u63 := (i64 * -1) & ~(1 << 63)
else:
    u63 := i64
```
This is rejoined into a u64 with neg in bit 6:
```
u64 := (neg << 6) | ((u63 & ~0x3f) << 1) | (u63 & 0x3f)
```
This value is then encoded using u64_dyn. Decoding is complicated slightly by the fact that `I64_MIN` is encoded as effectively `-0`.

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
