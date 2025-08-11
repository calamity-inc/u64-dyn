package u64dyn

import "errors"

// PackU64Dyn packs an integer into u64_dyn bytes.
func PackU64Dyn(v uint64) []byte {
    out := make([]byte, 0, 9)
    for i := 0; i < 8; i++ {
        cur := byte(v & 0x7f)
        v >>= 7
        if v != 0 {
            out = append(out, cur|0x80)
        } else {
            out = append(out, cur)
            return out
        }
    }
    if v != 0 {
        out = append(out, byte(v))
    }
    return out
}

// UnpackU64Dyn unpacks a u64_dyn encoded integer from buf starting at offset.
// It returns the value, the new offset, or an error if there is insufficient data.
func UnpackU64Dyn(buf []byte, offset int) (uint64, int, error) {
    var v uint64
    bits := uint(0)
    used := 0
    for used < 8 {
        if offset+used >= len(buf) {
            return 0, offset, errors.New("insufficient data")
        }
        b := uint64(buf[offset+used])
        v += (b & 0x7f) << bits
        used++
        if (b & 0x80) == 0 {
            return v, offset + used, nil
        }
        bits += 7
    }
    if offset+used >= len(buf) {
        return 0, offset, errors.New("insufficient data")
    }
    b := uint64(buf[offset+used])
    v += b << 56
    used++
    return v, offset + used, nil
}

// PackU64DynV2 packs an integer using u64_dyn_v2.
func PackU64DynV2(v uint64) []byte {
    out := make([]byte, 0, 9)
    for i := 0; i < 8; i++ {
        cur := byte(v & 0x7f)
        v >>= 7
        if v != 0 {
            out = append(out, cur|0x80)
            v--
        } else {
            out = append(out, cur)
            return out
        }
    }
    if v != 0 {
        out = append(out, byte(v))
    }
    return out
}

// UnpackU64DynV2 unpacks a u64_dyn_v2 encoded integer from buf starting at offset.
// It returns the value, the new offset, or an error if there is insufficient data.
func UnpackU64DynV2(buf []byte, offset int) (uint64, int, error) {
    var v uint64
    bits := uint(0)
    used := 0
    for used < 8 {
        if offset+used >= len(buf) {
            return 0, offset, errors.New("insufficient data")
        }
        b := uint64(buf[offset+used])
        v += (b & 0x7f) << bits
        used++
        if (b & 0x80) == 0 {
            return v, offset + used, nil
        }
        bits += 7
        v += 1 << bits
    }
    if offset+used >= len(buf) {
        return 0, offset, errors.New("insufficient data")
    }
    b := uint64(buf[offset+used])
    v += b << 56
    used++
    return v, offset + used, nil
}

// PackI64Dyn packs a signed integer using i64_dyn.
func PackI64Dyn(v int64) []byte {
    var u uint64 = uint64(v)
    neg := uint64(0)
    if v < 0 {
        neg = 1
        u = (^u + 1) & ^(uint64(1) << 63)
    }
    packed := (neg << 6) | ((u & ^uint64(0x3f)) << 1) | (u & 0x3f)
    return PackU64Dyn(packed)
}

// UnpackI64Dyn unpacks an i64_dyn encoded integer from buf starting at offset.
// It returns the value, the new offset, or an error if there is insufficient data.
func UnpackI64Dyn(buf []byte, offset int) (int64, int, error) {
    u64, idx, err := UnpackU64Dyn(buf, offset)
    if err != nil {
        return 0, offset, err
    }
    neg := (u64 >> 6) & 1
    u := ((u64 >> 1) & ^uint64(0x3f)) | (u64 & 0x3f)
    var v int64 = int64(u)
    if neg != 0 {
        v = int64((^(u - 1)) | (uint64(1) << 63))
    }
    return v, idx, nil
}

// PackI64DynV2 packs a signed integer using i64_dyn_v2.
func PackI64DynV2(v int64) []byte {
    var u uint64
    var neg uint64
    if v < 0 {
        neg = 1
        u = uint64(-v) - 1
    } else {
        u = uint64(v)
    }
    packed := (neg << 6) | ((u & ^uint64(0x3f)) << 1) | (u & 0x3f)
    return PackU64DynV2(packed)
}

// UnpackI64DynV2 unpacks an i64_dyn_v2 encoded integer from buf starting at offset.
// It returns the value, the new offset, or an error if there is insufficient data.
func UnpackI64DynV2(buf []byte, offset int) (int64, int, error) {
    u64, idx, err := UnpackU64DynV2(buf, offset)
    if err != nil {
        return 0, offset, err
    }
    neg := (u64 >> 6) & 1
    u := ((u64 >> 1) & ^uint64(0x3f)) | (u64 & 0x3f)
    var v int64
    if neg != 0 {
        v = -int64(u) - 1
    } else {
        v = int64(u)
    }
    return v, idx, nil
}
