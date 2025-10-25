package u64dyn

import (
	"bytes"
	"testing"
)

func TestPackUnpackU64(t *testing.T) {
	cases := map[uint64][]byte{
		0:                  {0x00},
		0x7F:               {0x7F},
		0x80:               {0x80, 0x01},
		1337:               {0xB9, 0x0A},
		42069:              {0xD5, 0xC8, 0x02},
		0xFFFFFFFFFFFFFFFF: {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
		0x8000000000000000: {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
	}
	for val, enc := range cases {
		got := PackU64Dyn(val)
		if !bytes.Equal(got, enc) {
			t.Errorf("PackU64Dyn(%d) = %v, want %v", val, got, enc)
		}
		v, off, err := UnpackU64Dyn(enc, 0)
		if err != nil {
			t.Fatalf("UnpackU64Dyn returned error: %v", err)
		}
		if v != val || off != len(enc) {
			t.Errorf("UnpackU64Dyn(%v) = (%d,%d), want (%d,%d)", enc, v, off, val, len(enc))
		}
	}
}

func TestPackUnpackI64(t *testing.T) {
	cases := map[int64][]byte{
		0:                   {0x00},
		0x7F:                {0xBF, 0x01},
		0x80:                {0x80, 0x02},
		1337:                {0xB9, 0x14},
		42069:               {0x95, 0x91, 0x05},
		-1:                  {0x41},
		-0x8000000000000000: {0x40},
	}
	for val, enc := range cases {
		got := PackI64Dyn(val)
		if !bytes.Equal(got, enc) {
			t.Errorf("PackI64Dyn(%d) = %v, want %v", val, got, enc)
		}
		v, off, err := UnpackI64Dyn(enc, 0)
		if err != nil {
			t.Fatalf("UnpackI64Dyn returned error: %v", err)
		}
		if v != val || off != len(enc) {
			t.Errorf("UnpackI64Dyn(%v) = (%d,%d), want (%d,%d)", enc, v, off, val, len(enc))
		}
	}
}

func TestPackUnpackU64V2(t *testing.T) {
	cases := map[uint64][]byte{
		0:                  {0x00},
		0x7F:               {0x7F},
                0x80:               {0x80, 0x00},
                1337:               {0xB9, 0x09},
                42069:              {0xD5, 0xC7, 0x01},
                0x123456789ABCDEF: {0xEF, 0x9A, 0xAE, 0xCC, 0xF7, 0xAB, 0xD0, 0x90, 0x00},
                0xFFFFFFFFFFFFFFFF: {0xFF, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE},
		0x8000000000000000: {0x80, 0xFF, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0x7E},
	}
	for val, enc := range cases {
		got := PackU64DynV2(val)
		if !bytes.Equal(got, enc) {
			t.Errorf("PackU64DynV2(%d) = %v, want %v", val, got, enc)
		}
		v, off, err := UnpackU64DynV2(enc, 0)
		if err != nil {
			t.Fatalf("UnpackU64DynV2 returned error: %v", err)
		}
		if v != val || off != len(enc) {
			t.Errorf("UnpackU64DynV2(%v) = (%d,%d), want (%d,%d)", enc, v, off, val, len(enc))
		}
	}
}

func TestPackUnpackI64V2(t *testing.T) {
	cases := map[int64][]byte{
		0:                   {0x00},
		0x7F:                {0xBF, 0x00},
		0x80:                {0x80, 0x01},
		1337:                {0xB9, 0x13},
		42069:               {0x95, 0x90, 0x04},
		-1:                  {0x40},
		-0x8000000000000000: {0xFF, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE},
	}
	for val, enc := range cases {
		got := PackI64DynV2(val)
		if !bytes.Equal(got, enc) {
			t.Errorf("PackI64DynV2(%d) = %v, want %v", val, got, enc)
		}
		v, off, err := UnpackI64DynV2(enc, 0)
		if err != nil {
			t.Fatalf("UnpackI64DynV2 returned error: %v", err)
		}
		if v != val || off != len(enc) {
			t.Errorf("UnpackI64DynV2(%v) = (%d,%d), want (%d,%d)", enc, v, off, val, len(enc))
		}
	}
}

func TestTruncated(t *testing.T) {
	truncated := [][]byte{
		{},
		{0x80},
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
	}
	for _, enc := range truncated {
		if _, _, err := UnpackU64Dyn(enc, 0); err == nil {
			t.Errorf("UnpackU64Dyn should fail for %v", enc)
		}
		if _, _, err := UnpackI64Dyn(enc, 0); err == nil {
			t.Errorf("UnpackI64Dyn should fail for %v", enc)
		}
		if _, _, err := UnpackU64DynV2(enc, 0); err == nil {
			t.Errorf("UnpackU64DynV2 should fail for %v", enc)
		}
		if _, _, err := UnpackI64DynV2(enc, 0); err == nil {
			t.Errorf("UnpackI64DynV2 should fail for %v", enc)
		}
	}
}

func TestWrapModulo64(t *testing.T) {
	enc := []byte{0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE}
	v, off, err := UnpackU64DynV2(enc, 0)
	if err != nil || v != 0x7F || off != len(enc) {
		t.Fatalf("UnpackU64DynV2 modulo check failed: (%d,%d,%v)", v, off, err)
	}
	sv, off, err := UnpackI64DynV2(enc, 0)
	if err != nil || sv != -64 || off != len(enc) {
		t.Fatalf("UnpackI64DynV2 modulo check failed: (%d,%d,%v)", sv, off, err)
	}
}
