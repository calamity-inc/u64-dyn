pub fn pack_u64_dyn(mut v: u64) -> Vec<u8> {
    let mut out = Vec::with_capacity(9);
    for _ in 0..8 {
        let cur = (v & 0x7f) as u8;
        v >>= 7;
        if v != 0 {
            out.push(cur | 0x80);
        } else {
            out.push(cur);
            return out;
        }
    }
    out.push(v as u8);
    out
}

pub fn unpack_u64_dyn(data: &[u8]) -> Option<(u64, usize)> {
    let mut v = 0u64;
    let mut bits = 0;
    let mut used = 0;
    loop {
        if used >= data.len() {
            return None;
        }
        let b = data[used] as u64;
        used += 1;
        if used == 9 {
            v = v.wrapping_add(b << bits);
            break;
        }
        v = v.wrapping_add((b & 0x7f) << bits);
        if (b & 0x80) == 0 {
            break;
        }
        bits += 7;
    }
    Some((v, used))
}

pub fn pack_u64_dyn_b(mut v: u64) -> Vec<u8> {
    let mut out = Vec::with_capacity(9);
    for _ in 0..8 {
        let cur = (v & 0x7f) as u8;
        v >>= 7;
        if v != 0 {
            out.push(cur | 0x80);
            v -= 1; // v2
        } else {
            out.push(cur);
            return out;
        }
    }
    out.push(v as u8);
    out
}

pub fn unpack_u64_dyn_b(data: &[u8]) -> Option<(u64, usize)> {
    let mut v = 0u64;
    let mut bits = 0;
    let mut used = 0;
    loop {
        if used >= data.len() {
            return None;
        }
        let b = data[used] as u64;
        used += 1;
        if used == 9 {
            v = v.wrapping_add(b << bits);
            break;
        }
        v = v.wrapping_add((b & 0x7f) << bits);
        if (b & 0x80) == 0 {
            break;
        }
        bits += 7;
        v = v.wrapping_add(1u64 << bits); // v2
    }
    Some((v, used))
}

pub fn pack_i64_dyn_a(v: i64) -> Vec<u8> {
    let mut u = v as u64;
    let neg = (v < 0) as u64;
    if neg != 0 {
        u = (!u).wrapping_add(1) & !(1u64 << 63);
    }
    let packed = (neg << 6) | ((u & !0x3f) << 1) | (u & 0x3f);
    pack_u64_dyn(packed)
}

pub fn unpack_i64_dyn_a(data: &[u8]) -> Option<(i64, usize)> {
    let (mut u, used) = unpack_u64_dyn(data)?;
    let neg = (u >> 6) & 1;
    u = ((u >> 1) & !0x3f) | (u & 0x3f);
    let mut v = u as i64;
    if neg != 0 {
        v = (!(u.wrapping_sub(1)) | (1u64 << 63)) as i64;
    }
    Some((v, used))
}

pub fn pack_i64_dyn_b(v: i64) -> Vec<u8> {
    let mut u = v as u64;
    let neg = (v < 0) as u64;
    if neg != 0 {
        u = !u;
    }
    let packed = (neg << 6) | ((u & !0x3f) << 1) | (u & 0x3f);
    pack_u64_dyn_b(packed)
}

pub fn unpack_i64_dyn_b(data: &[u8]) -> Option<(i64, usize)> {
    let (mut u, used) = unpack_u64_dyn_b(data)?;
    let neg = (u >> 6) & 1;
    u = ((u >> 1) & !0x3f) | (u & 0x3f);
    let mut v = u as i64;
    if neg != 0 {
        v = !v;
    }
    Some((v, used))
}

#[deprecated(note = "pack_u64_dyn_v2 has been renamed to pack_u64_dyn_b")]
pub fn pack_u64_dyn_v2(v: u64) -> Vec<u8> {
    pack_u64_dyn_b(v)
}

#[deprecated(note = "unpack_u64_dyn_v2 has been renamed to unpack_u64_dyn_b")]
pub fn unpack_u64_dyn_v2(data: &[u8]) -> Option<(u64, usize)> {
    unpack_u64_dyn_b(data)
}

#[deprecated(note = "pack_i64_dyn has been renamed to pack_i64_dyn_a")]
pub fn pack_i64_dyn(v: i64) -> Vec<u8> {
    pack_i64_dyn_a(v)
}

#[deprecated(note = "unpack_i64_dyn has been renamed to unpack_i64_dyn_a")]
pub fn unpack_i64_dyn(data: &[u8]) -> Option<(i64, usize)> {
    unpack_i64_dyn_a(data)
}

#[deprecated(note = "pack_i64_dyn_v2 has been renamed to pack_i64_dyn_b")]
pub fn pack_i64_dyn_v2(v: i64) -> Vec<u8> {
    pack_i64_dyn_b(v)
}

#[deprecated(note = "unpack_i64_dyn_v2 has been renamed to unpack_i64_dyn_b")]
pub fn unpack_i64_dyn_v2(data: &[u8]) -> Option<(i64, usize)> {
    unpack_i64_dyn_b(data)
}
