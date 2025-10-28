<?php
function add64($a, $b)
{
    $mask = -1;
    while ($b != 0)
    {
        $carry = ($a & $b) << 1;
        $a = ($a ^ $b) & $mask;
        $b = $carry & $mask;
    }
    return $a;
}

function pack_u64_dyn($v)
{
    if (is_float($v))
    {
        throw new Exception("Cannot encode a float as u64");
    }
    $out = "";
    for ($i = 0; $i != 8; ++$i)
    {
        $cur = $v & 0x7f;
        $v >>= 7;
        if ($v != 0)
        {
            $out .= chr($cur | 0x80);
        }
        else
        {
            $out .= chr($cur);
            return $out;
        }
    }
    $out .= chr($v);
    return $out;
}

function unpack_u64_dyn($str, &$offset = 0)
{
    $len = strlen($str);
    $v = 0;
    $bits = 0;
    $i = $offset;
    while ($i < $len && $i - $offset < 8)
    {
        $b = ord($str[$i]);
        $i++;
        $v = add64($v, ($b & 0x7f) << $bits);
        if (($b & 0x80) == 0)
        {
            $offset = $i;
            return $v;
        }
        $bits += 7;
    }
    if ($i >= $len)
    {
        throw new Exception("Insufficient data");
    }
    $b = ord($str[$i]);
    $v = add64($v, $b << 56);
    $i++;
    $offset = $i;
    return $v;
}

function pack_u64_dyn_p($v)
{
    if (is_float($v))
    {
        throw new Exception("Cannot encode a float as u64");
    }

    $byte_length = 1;
    if (($v >> 7) != 0) { $byte_length += 1; }
    if (($v >> 14) != 0) { $byte_length += 1; }
    if (($v >> 21) != 0) { $byte_length += 1; }
    if (($v >> 28) != 0) { $byte_length += 1; }
    if (($v >> 35) != 0) { $byte_length += 1; }
    if (($v >> 42) != 0) { $byte_length += 1; }
    if (($v >> 49) != 0) { $byte_length += 1; }
    if (($v >> 56) != 0) { $byte_length += 1; }

    $first_byte_value_bits = $byte_length < 8 ? 8 - $byte_length : 0;
    $first_byte_prefix_bits = $byte_length - 1;

    if ($first_byte_prefix_bits == 0)
    {
        $prefix = 0;
    }
    else
    {
        $prefix = ((1 << $first_byte_prefix_bits) - 1) << (8 - $first_byte_prefix_bits);
    }

    $value_mask = $first_byte_value_bits == 0 ? 0 : ((1 << $first_byte_value_bits) - 1);
    $first_byte = ($prefix | ($v & $value_mask)) & 0xff;
    if ($first_byte_value_bits != 0)
    {
        $v >>= $first_byte_value_bits;
    }

    $out = chr($first_byte);
    for ($idx = 1; $idx < $byte_length; ++$idx)
    {
        $out .= chr(($v >> (($idx - 1) * 8)) & 0xff);
    }

    return $out;
}

function unpack_u64_dyn_p($str, &$offset = 0)
{
    $len = strlen($str);
    if ($offset >= $len)
    {
        throw new Exception("Insufficient data");
    }

    $first_byte = ord($str[$offset]);

    $prefix_bits = 0;
    while ($prefix_bits < 8 && ($first_byte & (0x80 >> $prefix_bits)) != 0)
    {
        $prefix_bits += 1;
    }

    $byte_length = $prefix_bits + 1;
    $first_byte_value_bits = $byte_length < 8 ? 8 - $byte_length : 0;

    if ($offset + $byte_length > $len)
    {
        throw new Exception("Insufficient data");
    }

    $v = 0;
    for ($idx = 1; $idx < $byte_length; ++$idx)
    {
        $b = ord($str[$offset + $idx]);
        $v |= $b << (($idx - 1) * 8);
    }

    if ($first_byte_value_bits != 0)
    {
        $v <<= $first_byte_value_bits;
        $v |= $first_byte & ((1 << $first_byte_value_bits) - 1);
    }

    $offset += $byte_length;
    return $v;
}

function pack_u64_dyn_b($v)
{
    if (is_float($v))
    {
        throw new Exception("Cannot encode a float as u64");
    }
    $out = "";
    for ($i = 0; $i != 8; ++$i)
    {
        $cur = $v & 0x7f;
        $v >>= 7;
        if ($v != 0)
        {
            $out .= chr($cur | 0x80);
            $v -= 1; // v2
        }
        else
        {
            $out .= chr($cur);
            return $out;
        }
    }
    $out .= chr($v);
    return $out;
}

function unpack_u64_dyn_b($str, &$offset = 0)
{
    $len = strlen($str);
    $v = 0;
    $bits = 0;
    $i = $offset;
    while ($i < $len && $i - $offset < 8)
    {
        $b = ord($str[$i]);
        $i++;
        $v = add64($v, ($b & 0x7f) << $bits);
        if (($b & 0x80) == 0)
        {
            $offset = $i;
            return $v;
        }
        $bits += 7;
        $v = add64($v, 1 << $bits); // v2
    }
    if ($i >= $len)
    {
        throw new Exception("Insufficient data");
    }
    $b = ord($str[$i]);
    $v = add64($v, $b << 56);
    $i++;
    $offset = $i;
    return $v;
}

function pack_i64_dyn_a($v)
{
    if (is_float($v))
    {
        throw new Exception("Cannot encode a float as i64");
    }
    $neg = ($v >> 63) & 1;
    if ($v < 0)
    {
        $u = add64(~$v, 1) & ~(-1 << 63);
    }
    else
    {
        $u = $v;
    }
    return pack_u64_dyn(($neg << 6) | (($u & ~0x3f) << 1) | ($u & 0x3f));
}

function unpack_i64_dyn_a($str, &$offset = 0)
{
    $v = unpack_u64_dyn($str, $offset);
    $neg = (($v >> 6) & 1) != 0;
    $upper = $v & ~0x7f;
    $upper = ($upper >> 1) & ~(-1 << 63);
    $v = $upper | ($v & 0x3f);
    if ($neg)
    {
        $v = (~($v - 1)) | (1 << 63);
    }
    return $v;
}

function pack_i64_dyn_b($v)
{
    if (is_float($v))
    {
        throw new Exception("Cannot encode a float as i64");
    }
    $neg = ($v >> 63) & 1;
    if ($v < 0)
    {
        $v = ~$v;
    }
    return pack_u64_dyn_b(($neg << 6) | (($v & ~0x3f) << 1) | ($v & 0x3f));
}

function unpack_i64_dyn_b($str, &$offset = 0)
{
    $v = unpack_u64_dyn_b($str, $offset);
    $neg = (($v >> 6) & 1) != 0;
    $upper = $v & ~0x7f;
    $upper = ($upper >> 1) & ~(-1 << 63);
    $v = $upper | ($v & 0x3f);
    if ($neg)
    {
        $v = ~$v;
    }
    return $v;
}

function warn_deprecated($old, $new)
{
    if (function_exists('trigger_error'))
    {
        @trigger_error("$old: renamed to $new", E_USER_DEPRECATED);
    }
}

function pack_u64_dyn_v2($v)
{
    warn_deprecated('pack_u64_dyn_v2', 'pack_u64_dyn_b');
    return pack_u64_dyn_b($v);
}

function unpack_u64_dyn_v2($str, &$offset = 0)
{
    warn_deprecated('unpack_u64_dyn_v2', 'unpack_u64_dyn_b');
    return unpack_u64_dyn_b($str, $offset);
}

function pack_i64_dyn($v)
{
    warn_deprecated('pack_i64_dyn', 'pack_i64_dyn_a');
    return pack_i64_dyn_a($v);
}

function unpack_i64_dyn($str, &$offset = 0)
{
    warn_deprecated('unpack_i64_dyn', 'unpack_i64_dyn_a');
    return unpack_i64_dyn_a($str, $offset);
}

function pack_i64_dyn_v2($v)
{
    warn_deprecated('pack_i64_dyn_v2', 'pack_i64_dyn_b');
    return pack_i64_dyn_b($v);
}

function unpack_i64_dyn_v2($str, &$offset = 0)
{
    warn_deprecated('unpack_i64_dyn_v2', 'unpack_i64_dyn_b');
    return unpack_i64_dyn_b($str, $offset);
}
