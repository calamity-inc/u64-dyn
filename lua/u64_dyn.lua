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
    out[#out + 1] = string.char(v)
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

function pack_i64_dyn_a(v)
    local neg = (v >> 63)
    if v < 0 then
        v = (~v + 1) & ~(1 << 63)
    end
    return pack_u64_dyn((neg << 6) | ((v & ~0x3f) << 1) | (v & 0x3f))
end

function unpack_i64_dyn_a(str, i)
    local v
    v, i = unpack_u64_dyn(str, i)
    local neg = ((v >> 6) & 1) ~= 0
    v = ((v >> 1) & ~0x3f) | (v & 0x3f)
    if neg then
        v = ~(v - 1) | (1 << 63)
    end
    return v, i
end

function pack_u64_dyn_b(v)
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
    out[#out + 1] = string.char(v)
    return table.concat(out)
end

function unpack_u64_dyn_b(str, i)
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

function pack_i64_dyn_b(v)
    local neg = (v >> 63)
    if v < 0 then
        v = ~v
    end
    return pack_u64_dyn_b((neg << 6) | ((v & ~0x3f) << 1) | (v & 0x3f))
end

function unpack_i64_dyn_b(str, i)
    local v
    v, i = unpack_u64_dyn_b(str, i)
    local neg = ((v >> 6) & 1) ~= 0
    v = ((v >> 1) & ~0x3f) | (v & 0x3f)
    if neg then
        v = ~v
    end
    return v, i
end

function pack_i64_dyn(v)
    if warn then
        warn("unpack_u64_dyn_b: renamed to pack_i64_dyn_a")
    end
    return pack_i64_dyn_a(v)
end
function unpack_i64_dyn(str, i)
    if warn then
        warn("unpack_i64_dyn: renamed to unpack_i64_dyn_a")
    end
    return unpack_i64_dyn_a(str, i)
end
function pack_u64_dyn_v2(v)
    if warn then
        warn("pack_u64_dyn_v2: renamed to pack_u64_dyn_b")
    end
    return pack_u64_dyn_b(v)
end
function unpack_u64_dyn_v2(str, i)
    if warn then
        warn("unpack_u64_dyn_v2: renamed to unpack_u64_dyn_b")
    end
    return unpack_u64_dyn_b(str, i)
end
function pack_i64_dyn_v2(v)
    if warn then
        warn("pack_i64_dyn_v2: renamed to pack_i64_dyn_b")
    end
    return pack_i64_dyn_b(v)
end
function unpack_i64_dyn_v2(str, i)
    if warn then
        warn("unpack_i64_dyn_v2: renamed to unpack_i64_dyn_b")
    end
    return unpack_i64_dyn_b(str, i)
end
