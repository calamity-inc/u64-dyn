#include "u64_dyn.hpp"

#include <cassert>
#include <climits>
#include <cstdint>
#include <cstring>
#include <stdexcept>

struct UPair {
  uint64_t v;
  const char *d;
  size_t s;
};
struct IPair {
  int64_t v;
  const char *d;
  size_t s;
};

#define COUNT(x) (sizeof(x) / sizeof(0 [x]))

int main() {
  uint8_t data[9];
  size_t out_size;
  {
    UPair pairs[] = {
        {0, "\x00", 1},
        {0x7f, "\x7F", 1},
        {0x80, "\x80\x00", 2},
        {1337, "\xB9\x09", 2},
        {42069, "\xD5\xC7\x01", 3},
        {0xffffffffffffffffULL, "\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE", 9},
        {0x8000000000000000ULL, "\x80\xFF\xFE\xFE\xFE\xFE\xFE\xFE\x7E", 9},
    };
    for (size_t i = 0; i != COUNT(pairs); ++i) {
      const UPair *pair = &pairs[i];
      assert(pack_u64_dyn<2>(data, pair->v) == pair->s);
      assert(std::memcmp(data, pair->d, pair->s) == 0);
      uint64_t out_v = unpack_u64_dyn<2>(data, pair->s, &out_size);
      assert(out_v == pair->v);
    }
  }
  {
    UPair pairs[] = {
        {0, "\x00", 1},
        {0x7f, "\x7F", 1},
        {0x80, "\x80\x01", 2},
        {1337, "\xB9\x0A", 2},
        {42069, "\xD5\xC8\x02", 3},
        {0xffffffffffffffffULL, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 9},
        {0x8000000000000000ULL, "\x80\x80\x80\x80\x80\x80\x80\x80\x80", 9},
    };
    for (size_t i = 0; i != COUNT(pairs); ++i) {
      const UPair *pair = &pairs[i];
      assert(pack_u64_dyn<1>(data, pair->v) == pair->s);
      assert(std::memcmp(data, pair->d, pair->s) == 0);
      uint64_t out_v = unpack_u64_dyn<1>(data, pair->s, &out_size);
      assert(out_v == pair->v);
    }
  }
  {
    IPair pairs[] = {
        {0, "\x00", 1},
        {0x7f, "\xBF\x01", 2},
        {0x80, "\x80\x02", 2},
        {1337, "\xB9\x14", 2},
        {42069, "\x95\x91\x05", 3},
        {-1, "\x41", 1},
        {INT64_MIN, "\x40", 1},
    };
    for (size_t i = 0; i != COUNT(pairs); ++i) {
      const IPair *pair = &pairs[i];
      assert(pack_i64_dyn<1>(data, pair->v) == pair->s);
      assert(std::memcmp(data, pair->d, pair->s) == 0);
      int64_t out_v = unpack_i64_dyn<1>(data, pair->s, &out_size);
      assert(out_v == pair->v);
    }
  }
  {
    IPair pairs[] = {
        {0, "\x00", 1},
        {0x7f, "\xBF\x00", 2},
        {0x80, "\x80\x01", 2},
        {1337, "\xB9\x13", 2},
        {42069, "\x95\x90\x04", 3},
        {-1, "\x40", 1},
        {INT64_MIN, "\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE", 9},
    };
    for (size_t i = 0; i != COUNT(pairs); ++i) {
      const IPair *pair = &pairs[i];
      assert(pack_i64_dyn<2>(data, pair->v) == pair->s);
      assert(std::memcmp(data, pair->d, pair->s) == 0);
      int64_t out_v = unpack_i64_dyn<2>(data, pair->s, &out_size);
      assert(out_v == pair->v);
    }
  }
  {
    const uint8_t bad1[] = {0x80};
    const uint8_t bad2[] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};
    struct {
      const uint8_t *d;
      size_t s;
    } bads[] = {
        {nullptr, 0},
        {bad1, sizeof(bad1)},
        {bad2, sizeof(bad2)},
    };
    for (size_t i = 0; i != COUNT(bads); ++i) {
      size_t used;
      bool threw;
      try {
        (void)unpack_u64_dyn<1>(bads[i].d, bads[i].s, &used);
        threw = false;
      } catch (const std::exception &) {
        threw = true;
      }
      assert(threw);
      try {
        (void)unpack_i64_dyn<1>(bads[i].d, bads[i].s, &used);
        threw = false;
      } catch (const std::exception &) {
        threw = true;
      }
      assert(threw);
      try {
        (void)unpack_i64_dyn<2>(bads[i].d, bads[i].s, &used);
        threw = false;
      } catch (const std::exception &) {
        threw = true;
      }
      assert(threw);
      try {
        (void)unpack_u64_dyn<2>(bads[i].d, bads[i].s, &used);
        threw = false;
      } catch (const std::exception &) {
        threw = true;
      }
      assert(threw);
    }
  }
  {
    const uint8_t enc[] = {0xFF, 0xFF, 0xFE, 0xFE, 0xFE,
                           0xFE, 0xFE, 0xFE, 0xFE};
    size_t used;
    assert(unpack_u64_dyn<2>(enc, sizeof(enc), &used) == 0x7F &&
           used == sizeof(enc));
    assert(unpack_i64_dyn<2>(enc, sizeof(enc), &used) == -64 &&
           used == sizeof(enc));
  }
  return 0;
}
