-- https://gist.github.com/Sainan/02c3ac9cea5015341412c92feec95e56

function pack_u64_dyn(v)
    local out = {}
    for _ = 1, 8 do
        local cur = v & 0x7f
        v = v >> 7
        if v ~= 0 then
            out[#out + 1] = string.char(cur | 0x80)
        else
            out[#out + 1] = string.char(cur)
            return table.concat(out)
        end
    end
    if v ~= 0 then
        out[#out + 1] = string.char(v)
    end
    return table.concat(out)
end

function unpack_u64_dyn(str, i)
    i = i or 1
    local v = 0
    local bits = 0
    for _ = 1, 8 do
        local b = str:byte(i) i = i + 1
        v = v + ((b & 0x7f) << bits)
        if (b >> 7) == 0 then
            return v, i
        end
        bits = bits + 7
    end
    local b = str:byte(i) i = i + 1
    v = v + (b << 56)
    return v, i
end

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

function pack_i64_dyn(v)
    local neg = (v >> 63)
    if v < 0 then
        v = (v * -1) & ~(1 << 63)
    end
    return pack_u64_dyn((neg << 6) | ((v & ~0x3f) << 1) | (v & 0x3f))
end

function unpack_i64_dyn(str, i)
    local v
    v, i = unpack_u64_dyn(str, i)
    local neg = ((v >> 6) & 1) ~= 0
    v = ((v >> 1) & ~0x3f) | (v & 0x3f)
    if neg then
        if v == 0 then
            v = 1 << 63
        else
            v = v * -1
        end
    end
    return v, i
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

function pack_u64_dyn_v2(v)
    local out = {}
    for _ = 1, 8 do
        local cur = v & 0x7f
        v = v >> 7
        if v ~= 0 then
            out[#out + 1] = string.char(cur | 0x80)
            v = v - 1 -- v2
        else
            out[#out + 1] = string.char(cur)
            return table.concat(out)
        end
    end
    if v ~= 0 then
        out[#out + 1] = string.char(v)
    end
    return table.concat(out)
end

function unpack_u64_dyn_v2(str, i)
    i = i or 1
    local v = 0
    local bits = 0
    for _ = 1, 8 do
        local b = str:byte(i) i = i + 1
        v = v + ((b & 0x7f) << bits)
        if (b >> 7) == 0 then
            return v, i
        end
        bits = bits + 7
        v = v + (1 << bits) -- v2
    end
    local b = str:byte(i) i = i + 1
    v = v + (b << 56)
    return v, i
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

function pack_i64_dyn_v2(v)
    local neg = (v >> 63)
    if v < 0 then
        v = ~v
    end
    return pack_u64_dyn_v2((neg << 6) | ((v & ~0x3f) << 1) | (v & 0x3f))
end

function unpack_i64_dyn_v2(str, i)
    local v
    v, i = unpack_u64_dyn_v2(str, i)
    local neg = ((v >> 6) & 1) ~= 0
    v = ((v >> 1) & ~0x3f) | (v & 0x3f)
    if neg then
        v = ~v
    end
    return v, i
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
