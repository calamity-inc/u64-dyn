#include "u64_dyn.h"

#include <assert.h>
#include <string.h> // memcmp
//#include <stdio.h>

struct UPair { uint64_t v; const char* d; size_t s; };
struct IPair { int64_t v; const char* d; size_t s; };

#define COUNT(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

int main()
{
    uint8_t data[9];
    size_t out_size;
    {
        struct UPair pairs[] = {
            { 0, "\x00", 1 },
            { 0x7f, "\x7F", 1 },
            { 0x80, "\x80\x00", 2 },
            { 1337, "\xB9\x09", 2 },
            { 42069, "\xD5\xC7\x01", 3 },
            { 0xffffffffffffffff, "\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE", 9 },
            { 0x8000000000000000, "\x80\xFF\xFE\xFE\xFE\xFE\xFE\xFE\x7E", 9 },
        };
        for (size_t i = 0; i != COUNT(pairs); ++i)
        {
            const struct UPair* pair = &pairs[i];
            //printf("%llu\n", pair->v);
            assert(pack_u64_dyn_v2(data, pair->v) == pair->s);
            assert(memcmp(data, pair->d, pair->s) == 0);
            assert(unpack_u64_dyn_v2(data, pair->s, &out_size) == pair->v);
        }
    }
    {
        struct IPair pairs[] = {
            { 0, "\x00", 1 },
            { 0x7f, "\xBF\x00", 2 },
            { 0x80, "\x80\x01", 2 },
            { 1337, "\xB9\x13", 2 },
            { 42069, "\x95\x90\x04", 3 },
            //{ -1, "\x40", 1 },
            { INT64_MIN, "\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE", 9 },
        };
        for (size_t i = 0; i != COUNT(pairs); ++i)
        {
            const struct IPair* pair = &pairs[i];
            //printf("%lli\n", pair->v);
            assert(pack_i64_dyn_v2(data, pair->v) == pair->s);
            assert(memcmp(data, pair->d, pair->s) == 0);
            assert(unpack_i64_dyn_v2(data, pair->s, &out_size) == pair->v);
        }
    }
    return 0;
}
