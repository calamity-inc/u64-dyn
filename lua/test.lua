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
    [0x7f] = "\xBF\x01",
    [0x80] = "\x80\x02",
    [1337] = "\xB9\x14",
    [42069] = "\x95\x91\x05",
    [0xffffffffffffffff] = "\x41",
    [0x8000000000000000] = "\x40",
}) do
    assert(pack_i64_dyn(val) == enc)
    assert(unpack_i64_dyn(enc) == val)
end

for val, enc in pairs({
    [0] = "\x00",
    [0x7f] = "\x7F",
    [0x80] = "\x80\x00",
    [1337] = "\xB9\x09",
    [42069] = "\xD5\xC7\x01",
    [0xffffffffffffffff] = "\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE",
    [0x8000000000000000] = "\x80\xFF\xFE\xFE\xFE\xFE\xFE\xFE\x7E",
}) do
    assert(pack_u64_dyn_v2(val) == enc)
    assert(unpack_u64_dyn_v2(enc) == val)
end

for val, enc in pairs({
    [0] = "\x00",
    [0x7f] = "\xBF\x00",
    [0x80] = "\x80\x01",
    [1337] = "\xB9\x13",
    [42069] = "\x95\x90\x04",
    [0xffffffffffffffff] = "\x40",
    [0x8000000000000000] = "\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE",
}) do
    assert(pack_i64_dyn_v2(val) == enc)
    assert(unpack_i64_dyn_v2(enc) == val)
end

for _, enc in pairs({ "", "\x80", "\x80\x80\x80\x80\x80\x80\x80\x80" }) do
    assert(not pcall(unpack_u64_dyn, enc))
    assert(not pcall(unpack_i64_dyn, enc))
    assert(not pcall(unpack_i64_dyn_v2, enc))
    assert(not pcall(unpack_u64_dyn_v2, enc))
end

do
    local enc = "\xFF\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE"
    local v, off = unpack_u64_dyn_v2(enc)
    assert(v == 0x7F)
    assert(off == #enc + 1)
    local sv, off2 = unpack_i64_dyn_v2(enc)
    assert(sv == -64)
    assert(off2 == #enc + 1)
end
