#ifndef __U64_DYN_H
#define __U64_DYN_H

#include <stdbool.h>
#include <stddef.h> // size_t
#include <stdint.h>

size_t pack_u64_dyn(uint8_t out[9], uint64_t v);
bool unpack_u64_dyn(const uint8_t* in_data, size_t in_size, uint64_t* out_v, size_t* out_size);
size_t pack_u64_dyn_v2(uint8_t out[9], uint64_t v);
bool unpack_u64_dyn_v2(const uint8_t* in_data, size_t in_size, uint64_t* out_v, size_t* out_size);
size_t pack_i64_dyn(uint8_t out[9], int64_t v);
bool unpack_i64_dyn(const uint8_t* in_data, size_t in_size, int64_t* out_v, size_t* out_size);
size_t pack_i64_dyn_v2(uint8_t out[9], int64_t v);
bool unpack_i64_dyn_v2(const uint8_t* in_data, size_t in_size, int64_t* out_v, size_t* out_size);

#endif
