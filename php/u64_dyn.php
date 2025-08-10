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

function unpack_u64_dyn_v2($str)
{
    $b = ord($str[0]);
    $v = $b & 0x7f;
    $i = 1;
    $bits = 7;
    $has_next = ($b >> 7) != 0;
    while ($has_next)
    {
        $b = ord($str[$i]);
        $i++;
        $has_next = false;
        if ($bits < 56)
        {
            $has_next = ($b >> 7) != 0;
            $b = $b & 0x7f;
        }
        $v |= (($b + 1) << $bits);
        $bits += 7;
    }
    return $v;
}

$tests_u64 = [
    0 => "\x00",
    0x7f => "\x7F",
    0x80 => "\x80\x00",
    1337 => "\xB9\x09",
    42069 => "\xD5\xC7\x01",
    /*0xffffffffffffffff*/ -1 => "\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE",
];

foreach ($tests_u64 as $val => $enc)
{
    assert(pack_u64_dyn_v2($val) == $enc);
    assert(unpack_u64_dyn_v2($enc) == $val);
}
