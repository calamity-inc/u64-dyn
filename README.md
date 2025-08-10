u64_dyn, i64_dyn, u64_dyn_v2, and i64_dyn_v2 are variable-length codings of 64-bit integers that require less bytes for smaller integers. Notably, encoded values never exceed 9 bytes.

## Implementations

- [C](c)
- C++
  - [Soup](https://github.com/calamity-inc/Soup) has implementations for [reading](https://github.com/calamity-inc/Soup/blob/senpai/soup/Reader.cpp) and [writing](https://github.com/calamity-inc/Soup/blob/senpai/soup/Writer.cpp)
- [Lua](lua/u64_dyn.lua)
  - The [APM](https://github.com/PlutoLang/apm#readme) package for this can be found [here](https://gist.github.com/Sainan/02c3ac9cea5015341412c92feec95e56)
- [PHP](php/u64_dyn.php)
