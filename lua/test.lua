require("u64_dyn")

for val, enc in pairs({
    [0] = "\x00",
    [0x7f] = "\x7F",
    [0x80] = "\x80\x01",
    [1337] = "\xB9\x0A",
    [42069] = "\xD5\xC8\x02",
    [0xffffffffffffffff] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
    [0x8000000000000000] = "\x80\x80\x80\x80\x80\x80\x80\x80\x80",
}) do
    assert(pack_u64_dyn(val) == enc)
    assert(unpack_u64_dyn(enc) == val)
end

for val, enc in pairs({
    [0] = "\x00",
    [0x7f] = "\x7F",
    [0x80] = "\x80\x02",
    [1337] = "\xB9\x14",
    [42069] = "\xD5\x22\x05",
    [0xffffffffffffffff] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
    [0x8000000000000000] = "\xFF\x00\x00\x00\x00\x00\x00\x00\x80",
}) do
    assert(pack_u64_dyn_p(val) == enc)
    local v, off = unpack_u64_dyn_p(enc)
    assert(v == val)
    assert(off == #enc + 1)
end

for val, enc in pairs({
    [0] = "\x00",
    [0x7f] = "\x7F",
    [0x80] = "\x80\x00",
    [1337] = "\xB9\x12",
    [42069] = "\xD5\x1E\x03",
    [0xffffffffffffffff] = "\xFF\x7F\xBF\xDF\xEF\xF7\xFB\xFD\xFE",
    [0x8000000000000000] = "\xFF\x80\xBF\xDF\xEF\xF7\xFB\xFD\x7E",
}) do
    assert(pack_u64_dyn_bp(val) == enc)
    local v, off = unpack_u64_dyn_bp(enc)
    assert(v == val)
    assert(off == #enc + 1)
end

for val, enc in pairs({
    [0] = "\x00",
    [0x7f] = "\xBF\x01",
    [0x80] = "\x80\x02",
    [1337] = "\xB9\x14",
    [42069] = "\x95\x91\x05",
    [-1] = "\x41",
    [-9223372036854775808] = "\x40",
}) do
    assert(pack_i64_dyn_a(val) == enc)
    assert(unpack_i64_dyn_a(enc) == val)
end

for val, enc in pairs({
    [0] = "\x00",
    [0x7f] = "\x7F",
    [0x80] = "\x80\x00",
    [1337] = "\xB9\x09",
    [42069] = "\xD5\xC7\x01",
    [0x123456789ABCDEF] = "\xEF\x9A\xAE\xCC\xF7\xAB\xD0\x90\x00",
    [0xffffffffffffffff] = "\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE",
    [0x8000000000000000] = "\x80\xFF\xFE\xFE\xFE\xFE\xFE\xFE\x7E",
}) do
    assert(pack_u64_dyn_b(val) == enc)
    assert(unpack_u64_dyn_b(enc) == val)
end

for val, enc in pairs({
    [0] = "\x00",
    [0x7f] = "\xBF\x00",
    [0x80] = "\x80\x01",
    [1337] = "\xB9\x13",
    [42069] = "\x95\x90\x04",
    [-1] = "\x40",
    [-9223372036854775808] = "\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE",
}) do
    assert(pack_i64_dyn_b(val) == enc)
    assert(unpack_i64_dyn_b(enc) == val)
end

for val, enc in pairs({
    [0] = "\x00",
    [0x7f] = "\xBF\x00",
    [0x80] = "\x80\x02",
    [1337] = "\xB9\x26",
    [42069] = "\xD5\x40\x08",
    [-1] = "\x40",
    [-9223372036854775808] = "\xFF\x7F\xBF\xDF\xEF\xF7\xFB\xFD\xFE",
}) do
    assert(pack_i64_dyn_bp(val) == enc)
    assert(unpack_i64_dyn_bp(enc) == val)
end

for _, enc in pairs({ "", "\x80", "\x80\x80\x80\x80\x80\x80\x80\x80" }) do
    assert(not pcall(unpack_u64_dyn, enc))
    assert(not pcall(unpack_i64_dyn_a, enc))
    assert(not pcall(unpack_i64_dyn_b, enc))
    assert(not pcall(unpack_u64_dyn_b, enc))
end

for _, enc in pairs({ "", "\x80", "\xFF\x00\x00\x00\x00\x00\x00\x00" }) do
    assert(not pcall(unpack_u64_dyn_p, enc))
    assert(not pcall(unpack_u64_dyn_bp, enc))
end

do
    local enc = "\xFF\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE"
    assert(not pcall(unpack_u64_dyn_b, enc))
    assert(not pcall(unpack_u64_dyn_bp, enc))
    assert(not pcall(unpack_i64_dyn_b, enc))
    assert(not pcall(unpack_i64_dyn_bp, enc))
end
