# Python implementation of u64_dyn, u64_dyn_v2, i64_dyn, and i64_dyn_v2

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


# u64_dyn ------------------------------------------------------------------

def pack_u64_dyn(v: int) -> bytes:
    """Pack an integer into u64_dyn bytes."""
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
    if v:
        out.append(v)
    return bytes(out)


def unpack_u64_dyn(buf: bytes, offset: int = 0) -> Tuple[int, int]:
    """Unpack a u64_dyn encoded integer from buf starting at offset.

    Returns a tuple of (value, new_offset).
    Raises ValueError if there is insufficient data.
    """
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
            return v, offset + used
        bits += 7
    if offset + used >= length:
        raise ValueError('Insufficient data')
    b = buf[offset + used]
    v += b << 56
    used += 1
    return v, offset + used


# u64_dyn_v2 ----------------------------------------------------------------

def pack_u64_dyn_v2(v: int) -> bytes:
    """Pack an integer using u64_dyn_v2."""
    v &= (1 << 64) - 1
    out = bytearray()
    for _ in range(8):
        cur = v & 0x7F
        v >>= 7
        if v:
            out.append(cur | 0x80)
            v -= 1
        else:
            out.append(cur)
            return bytes(out)
    if v:
        out.append(v)
    return bytes(out)


def unpack_u64_dyn_v2(buf: bytes, offset: int = 0) -> Tuple[int, int]:
    """Unpack a u64_dyn_v2 encoded integer from buf starting at offset.

    Returns (value, new_offset).
    Raises ValueError if insufficient data.
    """
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
            return v, offset + used
        bits += 7
        v += 1 << bits
    if offset + used >= length:
        raise ValueError('Insufficient data')
    b = buf[offset + used]
    v += b << 56
    used += 1
    return v, offset + used


# i64_dyn -------------------------------------------------------------------

def pack_i64_dyn(v: int) -> bytes:
    """Pack a signed integer using i64_dyn."""
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
        v = (~(u - 1) | (1 << 63)) & ((1 << 64) - 1)
        if v & (1 << 63):
            v -= 1 << 64
    else:
        v = u
    return v, idx


# i64_dyn_v2 ----------------------------------------------------------------

def pack_i64_dyn_v2(v: int) -> bytes:
    """Pack a signed integer using i64_dyn_v2."""
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
