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

$tests_u64 = [
    0 => "\x00",
    0x7f => "\x7F",
    0x80 => "\x80\x00",
    1337 => "\xB9\x09",
    42069 => "\xD5\xC7\x01",
    /*0xffffffffffffffff*/ -1 => "\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFE",
    /*0x8000000000000000*/ ((-1) << 63) => "\x80\xFF\xFE\xFE\xFE\xFE\xFE\xFE\x7E",
];

foreach ($tests_u64 as $val => $enc)
{
    assert(pack_u64_dyn_v2($val) == $enc);
    assert(unpack_u64_dyn_v2($enc) == $val);
}
