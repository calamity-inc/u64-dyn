#include "u64_dyn.h"

#include <assert.h>
#include <stdio.h>
#include <string.h> // memcmp

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

#define COUNT(x)                                                               \
  ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x])))))

int main() {
  uint8_t data[9];
  size_t out_size;
  {
    struct UPair pairs[] = {
        {0, "\x00", 1},
        {0x7f, "\x7F", 1},
        {0x80, "\x80\x00", 2},
        {1337, "\xB9\x09", 2},
        {42069, "\xD5\xC7\x01", 3},
        {0x123456789ABCDEF, "\xEF\x9A\xAE\xCC\xF7\xAB\xD0\x90\x00", 9},
        {0xffffffffffffffff, "\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE", 9},
        {0x8000000000000000, "\x80\xFF\xFE\xFE\xFE\xFE\xFE\xFE\x7E", 9},
    };
    for (size_t i = 0; i != COUNT(pairs); ++i) {
      const struct UPair *pair = &pairs[i];
      // printf("%llu\n", pair->v);
      assert(pack_u64_dyn_b(data, pair->v) == pair->s);
      assert(memcmp(data, pair->d, pair->s) == 0);
      uint64_t out_v;
      assert(unpack_u64_dyn_b(data, pair->s, &out_v, &out_size));
      assert(out_v == pair->v);
      assert(out_size == pair->s);
    }
  }
  {
    struct UPair pairs[] = {
        {0, "\x00", 1},
        {0x7f, "\x7F", 1},
        {0x80, "\x80\x01", 2},
        {1337, "\xB9\x0A", 2},
        {42069, "\xD5\xC8\x02", 3},
        {0xffffffffffffffff, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 9},
        {0x8000000000000000, "\x80\x80\x80\x80\x80\x80\x80\x80\x80", 9},
    };
    for (size_t i = 0; i != COUNT(pairs); ++i) {
      const struct UPair *pair = &pairs[i];
      // printf("%llu\n", pair->v);
      assert(pack_u64_dyn(data, pair->v) == pair->s);
      assert(memcmp(data, pair->d, pair->s) == 0);
      uint64_t out_v;
      assert(unpack_u64_dyn(data, pair->s, &out_v, &out_size));
      assert(out_v == pair->v);
      assert(out_size == pair->s);
    }
  }
  {
    struct IPair pairs[] = {
        {0, "\x00", 1},
        {0x7f, "\xBF\x01", 2},
        {0x80, "\x80\x02", 2},
        {1337, "\xB9\x14", 2},
        {42069, "\x95\x91\x05", 3},
        {-1, "\x41", 1},
        {INT64_MIN, "\x40", 1},
    };
    for (size_t i = 0; i != COUNT(pairs); ++i) {
      const struct IPair *pair = &pairs[i];
      assert(pack_i64_dyn_a(data, pair->v) == pair->s);
      assert(memcmp(data, pair->d, pair->s) == 0);
      int64_t out_v;
      assert(unpack_i64_dyn_a(data, pair->s, &out_v, &out_size));
      assert(out_v == pair->v);
      assert(out_size == pair->s);
    }
  }
  {
    struct IPair pairs[] = {
        {0, "\x00", 1},
        {0x7f, "\xBF\x00", 2},
        {0x80, "\x80\x01", 2},
        {1337, "\xB9\x13", 2},
        {42069, "\x95\x90\x04", 3},
        {-1, "\x40", 1},
        {INT64_MIN, "\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE", 9},
    };
    for (size_t i = 0; i != COUNT(pairs); ++i) {
      const struct IPair *pair = &pairs[i];
      // printf("%lli\n", pair->v);
      assert(pack_i64_dyn_b(data, pair->v) == pair->s);
      assert(memcmp(data, pair->d, pair->s) == 0);
      int64_t out_v;
      assert(unpack_i64_dyn_b(data, pair->s, &out_v, &out_size));
      assert(out_v == pair->v);
      assert(out_size == pair->s);
    }
  }
  {
    struct UPair pairs[] = {
        {0, "\x00", 1},
        {0x7f, "\x7F", 1},
        {0x80, "\x80\x02", 2},
        {1337, "\xB9\x14", 2},
        {42069, "\xD5\x22\x05", 3},
        {0xffffffffffffffff, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 9},
        {0x8000000000000000, "\xFF\x00\x00\x00\x00\x00\x00\x00\x80", 9},
    };
    for (size_t i = 0; i != COUNT(pairs); ++i) {
      const struct UPair *pair = &pairs[i];
      // printf("%llu\n", pair->v);
      assert(pack_u64_dyn_p(data, pair->v) == pair->s);
      assert(memcmp(data, pair->d, pair->s) == 0);
      uint64_t out_v;
      assert(unpack_u64_dyn_p(data, pair->s, &out_v, &out_size));
      assert(out_v == pair->v);
      assert(out_size == pair->s);
    }
  }
  // Unfinished data
  {
    const uint8_t bad1[] = {0x80};
    const uint8_t bad2[] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80};
    struct {
      const uint8_t *d;
      size_t s;
    } bads[] = {
        {NULL, 0},
        {bad1, sizeof(bad1)},
        {bad2, sizeof(bad2)},
    };
    for (size_t i = 0; i != COUNT(bads); ++i) {
      uint64_t u;
      int64_t v;
      size_t used;
      assert(!unpack_u64_dyn(bads[i].d, bads[i].s, &u, &used));
      assert(!unpack_i64_dyn_a(bads[i].d, bads[i].s, &v, &used));
      assert(!unpack_i64_dyn_b(bads[i].d, bads[i].s, &v, &used));
      assert(!unpack_u64_dyn_b(bads[i].d, bads[i].s, &u, &used));
    }
  }
  {
    const uint8_t bad1[] = {0x80};
    const uint8_t bad2[] = {0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    struct {
      const uint8_t *d;
      size_t s;
    } bads[] = {
        {NULL, 0},
        {bad1, sizeof(bad1)},
        {bad2, sizeof(bad2)},
    };
    for (size_t i = 0; i != COUNT(bads); ++i) {
      uint64_t u;
      // int64_t v;
      size_t used;
      assert(!unpack_u64_dyn_p(bads[i].d, bads[i].s, &u, &used));
    }
  }
  // Invalid data
  {
    const uint8_t enc[] = {0xFF, 0xFF, 0xFE, 0xFE, 0xFE,
                           0xFE, 0xFE, 0xFE, 0xFE};
    uint64_t u;
    size_t used;
    assert(!unpack_u64_dyn_b(enc, sizeof(enc), &u, &used));
  }
  printf("All tests ran successfully.\n");
  return 0;
}
