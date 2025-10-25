from typing import Tuple

__all__ = [
    "pack_u64_dyn",
    "unpack_u64_dyn",
    "pack_u64_dyn_v2",
    "unpack_u64_dyn_v2",
    "pack_i64_dyn",
    "unpack_i64_dyn",
    "pack_i64_dyn_v2",
    "unpack_i64_dyn_v2",
]

def pack_u64_dyn(v: int) -> bytes:
    v &= (1 << 64) - 1
    out = bytearray()
    for _ in range(8):
        cur = v & 0x7F
        v >>= 7
        if v:
            out.append(cur | 0x80)
        else:
            out.append(cur)
            return bytes(out)
    out.append(v)
    return bytes(out)


def unpack_u64_dyn(buf: bytes, offset: int = 0) -> Tuple[int, int]:
    v = 0
    bits = 0
    used = 0
    length = len(buf)
    while used < 8:
        if offset + used >= length:
            raise ValueError('Insufficient data')
        b = buf[offset + used]
        v += (b & 0x7F) << bits
        used += 1
        if (b & 0x80) == 0:
            return v & ((1 << 64) - 1), offset + used
        bits += 7
    if offset + used >= length:
        raise ValueError('Insufficient data')
    b = buf[offset + used]
    v += b << 56
    used += 1
    return v & ((1 << 64) - 1), offset + used

def pack_u64_dyn_v2(v: int) -> bytes:
    v &= (1 << 64) - 1
    out = bytearray()
    for _ in range(8):
        cur = v & 0x7F
        v >>= 7
        if v:
            out.append(cur | 0x80)
            v -= 1 # v2
        else:
            out.append(cur)
            return bytes(out)
    out.append(v)
    return bytes(out)

def unpack_u64_dyn_v2(buf: bytes, offset: int = 0) -> Tuple[int, int]:
    v = 0
    bits = 0
    used = 0
    length = len(buf)
    while used < 8:
        if offset + used >= length:
            raise ValueError('Insufficient data')
        b = buf[offset + used]
        v += (b & 0x7F) << bits
        used += 1
        if (b & 0x80) == 0:
            return v & ((1 << 64) - 1), offset + used
        bits += 7
        v += 1 << bits # v2
    if offset + used >= length:
        raise ValueError('Insufficient data')
    b = buf[offset + used]
    v += b << 56
    used += 1
    return v & ((1 << 64) - 1), offset + used

def pack_i64_dyn(v: int) -> bytes:
    if v < 0:
        neg = 1
        u = (~v + 1) & ((1 << 63) - 1)
    else:
        neg = 0
        u = v
    packed = (neg << 6) | ((u & ~0x3F) << 1) | (u & 0x3F)
    return pack_u64_dyn(packed)

def unpack_i64_dyn(buf: bytes, offset: int = 0) -> Tuple[int, int]:
    u64, idx = unpack_u64_dyn(buf, offset)
    neg = (u64 >> 6) & 1
    u = ((u64 >> 1) & ~0x3F) | (u64 & 0x3F)
    if neg:
        v = ~(u - 1) | -9223372036854775808
    else:
        v = u
    return v, idx

def pack_i64_dyn_v2(v: int) -> bytes:
    if v < 0:
        neg = 1
        u = (-v) - 1
    else:
        neg = 0
        u = v
    packed = (neg << 6) | ((u & ~0x3F) << 1) | (u & 0x3F)
    return pack_u64_dyn_v2(packed)

def unpack_i64_dyn_v2(buf: bytes, offset: int = 0) -> Tuple[int, int]:
    u64, idx = unpack_u64_dyn_v2(buf, offset)
    neg = (u64 >> 6) & 1
    u = ((u64 >> 1) & ~0x3F) | (u64 & 0x3F)
    if neg:
        v = (-u) - 1
    else:
        v = u
    return v, idx
