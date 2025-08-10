<?php
function pack_u64_dyn_v2($v)
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
    if ($v != 0)
    {
        $out .= chr($v);
    }
    return $out;
}

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

function unpack_u64_dyn_v2($str)
{
    $len = strlen($str);
    $v = 0;
    $bits = 0;
    $i = 0;
    while ($i < $len && $i < 8)
    {
        $b = ord($str[$i]);
        $i++;
        $v = add64($v, ($b & 0x7f) << $bits);
        if (($b & 0x80) == 0)
        {
            return $v;
        }
        $bits += 7;
        $v = add64($v, 1 << $bits);
    }
    if ($i < $len)
    {
        $b = ord($str[$i]);
        $v = add64($v, $b << 56);
    }
    return $v;
}

function pack_i64_dyn_v2($v)
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
    return pack_u64_dyn_v2(($neg << 6) | (($v & ~0x3f) << 1) | ($v & 0x3f));
}

function unpack_i64_dyn_v2($str)
{
    $v = unpack_u64_dyn_v2($str);
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

$tests_u64 = [
    0 => "\x00",
    0x7f => "\x7F",
    0x80 => "\x80\x00",
    1337 => "\xB9\x09",
    42069 => "\xD5\xC7\x01",
    -1 => "\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE",
    -9223372036854775808 => "\x80\xFF\xFE\xFE\xFE\xFE\xFE\xFE\x7E",
];

foreach ($tests_u64 as $val => $enc)
{
    assert(pack_u64_dyn_v2($val) == $enc);
    assert(unpack_u64_dyn_v2($enc) == $val);
}

$tests_i64 = [
    0 => "\x00",
    0x7f => "\xBF\x00",
    0x80 => "\x80\x01",
    1337 => "\xB9\x13",
    42069 => "\x95\x90\x04",
    -1 => "\x40",
    -9223372036854775808 => "\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE",
];

foreach ($tests_i64 as $val => $enc)
{
    assert(pack_i64_dyn_v2($val) == $enc);
    assert(unpack_i64_dyn_v2($enc) == $val);
}
