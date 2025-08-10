#ifndef __U64_DYN_H
#define __U64_DYN_H

#include <stddef.h> // size_t
#include <stdint.h>

size_t pack_u64_dyn(uint8_t out[9], uint64_t v);
uint64_t unpack_u64_dyn(const uint8_t* in_data, size_t in_size, size_t* out_size);
size_t pack_u64_dyn_v2(uint8_t out[9], uint64_t v);
uint64_t unpack_u64_dyn_v2(const uint8_t* in_data, size_t in_size, size_t* out_size);
size_t pack_i64_dyn_v2(uint8_t out[9], int64_t v);
int64_t unpack_i64_dyn_v2(const uint8_t* in_data, size_t in_size, size_t* out_size);

#endif
