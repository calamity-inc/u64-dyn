package u64dyn

import (
	"errors"
)

var (
	ErrInsufficientData = errors.New("insufficient data")
)

// PackU64Dyn packs an integer into u64_dyn bytes.
func PackU64Dyn(in uint64) []byte {
	var buff [9]byte
	return packV1(in, &buff)
}

// UnpackU64Dyn unpacks a u64_dyn encoded integer from buf starting at offset.
// It returns the value, the new offset, or an error if there is insufficient data.
func UnpackU64Dyn(buf []byte, offset int) (result uint64, off int, err error) {
	var bits uint
	var used int
	bufLen := len(buf)

	for used < 8 {
		if offset+used >= bufLen {
			return 0, offset, ErrInsufficientData
		}
		b := uint64(buf[offset+used])
		result |= (b & 0x7f) << bits
		used++
		if (b & 0x80) == 0 {
			return result, offset + used, nil
		}
		bits += 7
	}
	if offset+used >= bufLen {
		return 0, offset, ErrInsufficientData
	}
	b := uint64(buf[offset+used])
	result |= b << 56
	used++
	return result, offset + used, nil
}

// PackU64DynV2 packs an integer using u64_dyn_v2.
func PackU64DynV2(in uint64) []byte {
	var buff [9]byte
	return packV2(in, &buff)
}

// UnpackU64DynV2 unpacks a u64_dyn_v2 encoded integer from buf starting at offset.
// It returns the value, the new offset, or an error if there is insufficient data.
func UnpackU64DynV2(buf []byte, offset int) (result uint64, off int, err error) {
	var bits uint
	var used int
	bufLen := len(buf)

	for used < 8 {
		if offset+used >= bufLen {
			return 0, offset, ErrInsufficientData
		}
		b := uint64(buf[offset+used])
		result += (b & 0x7f) << bits
		used++
		if (b & 0x80) == 0 {
			return result, offset + used, nil
		}
		bits += 7
		result += 1 << bits
	}
	if offset+used >= bufLen {
		return 0, offset, ErrInsufficientData
	}
	b := uint64(buf[offset+used])
	result += b << 56
	used++
	return result, offset + used, nil
}

// PackI64Dyn packs a signed integer using i64_dyn.
func PackI64Dyn(in int64) []byte {
	var buff [9]byte
	return packI64V1(in, &buff)
}

// UnpackI64Dyn unpacks an i64_dyn encoded integer from buf starting at offset.
// It returns the value, the new offset, or an error if there is insufficient data.
func UnpackI64Dyn(buf []byte, offset int) (result int64, off int, err error) {
	u, off, err := UnpackU64Dyn(buf, offset)
	if err != nil {
		return 0, off, err
	}

	sign := (u >> 6) & 1
	u63 := ((u >> 1) & ^uint64(0x3f)) | (u & 0x3f)

	if sign == 0 {
		return int64(u63), off, nil
	}

	if u63 == 0 {
		return -1 << 63, off, nil
	}

	return -int64(u63), off, nil
}

// PackI64DynV2 packs a signed integer using i64_dyn_v2.
func PackI64DynV2(in int64) []byte {
	var buff [9]byte
	return packI64V2(in, &buff)
}

// UnpackI64DynV2 unpacks an i64_dyn_v2 encoded integer from buf starting at offset.
// It returns the value, the new offset, or an error if there is insufficient data.
func UnpackI64DynV2(buf []byte, offset int) (result int64, off int, err error) {
	u, off, err := UnpackU64DynV2(buf, offset)
	if err != nil {
		return 0, off, err
	}

	sign := (u >> 6) & 1
	u63 := ((u >> 1) & ^uint64(0x3f)) | (u & 0x3f)

	if sign == 0 {
		return int64(u63), off, nil
	}

	return int64(^u63), off, nil
}

func packV1(in uint64, out *[9]byte) []byte {
	for i := 0; i < 8; i++ {
		cur := byte(in & 0x7f)
		in >>= 7
		if in != 0 {
			out[i] = cur | 0x80
		} else {
			out[i] = cur
			return out[:i+1]
		}
	}
	if in != 0 {
		out[8] = byte(in)
	}

	return out[:]
}

func packV2(in uint64, out *[9]byte) []byte {
	for i := 0; i < 8; i++ {
		cur := byte(in & 0x7f)
		in >>= 7
		if in != 0 {
			out[i] = cur | 0x80
			in--
		} else {
			out[i] = cur
			return out[:i+1]
		}
	}
	if in != 0 {
		out[8] = byte(in)
	}

	return out[:]
}

func packI64V1(in int64, out *[9]byte) []byte {
	neg := in < 0

	var u63 uint64
	if neg {
		u63 = uint64(-in) & ^(uint64(1) << 63)
	} else {
		u63 = uint64(in)
	}

	var sign uint64
	if neg {
		sign = 1
	}

	u := (sign << 6) | ((u63 & ^uint64(0x3f)) << 1) | (u63 & 0x3f)
	return packV1(u, out)
}

func packI64V2(in int64, out *[9]byte) []byte {
	neg := in < 0

	var u63 uint64
	if neg {
		u63 = uint64(^in)
	} else {
		u63 = uint64(in)
	}

	var sign uint64
	if neg {
		sign = 1
	}

	u := (sign << 6) | ((u63 & ^uint64(0x3f)) << 1) | (u63 & 0x3f)
	return packV2(u, out)
}
