package u64dyn

import "testing"

func BenchmarkU64Dyn(b *testing.B) {
	for i := 0; i < b.N; i++ {
		tmp := PackU64Dyn(uint64(i))
		_, _, _ = UnpackU64Dyn(tmp, 0)
	}
}

func BenchmarkI64Dyn(b *testing.B) {
	for i := 0; i < b.N; i++ {
		tmp := PackI64Dyn(int64(i))
		_, _, _ = UnpackI64Dyn(tmp, 0)
	}
}

func BenchmarkU64DynV2(b *testing.B) {
	for i := 0; i < b.N; i++ {
		tmp := PackU64DynV2(uint64(i))
		_, _, _ = UnpackU64DynV2(tmp, 0)
	}
}

func BenchmarkI64DynV2(b *testing.B) {
	for i := 0; i < b.N; i++ {
		tmp := PackI64DynV2(int64(i))
		_, _, _ = UnpackI64DynV2(tmp, 0)
	}
}
