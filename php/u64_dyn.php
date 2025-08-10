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
    if ($v != 0)
    {
        $out .= chr($v);
    }
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
        throw new Exception("Insufficient data to decode u64");
    }
    $b = ord($str[$i]);
    $v = add64($v, $b << 56);
    $i++;
    $offset = $i;
    return $v;
}

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

function unpack_u64_dyn_v2($str, &$offset = 0)
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
        throw new Exception("Insufficient data to decode u64");
    }
    $b = ord($str[$i]);
    $v = add64($v, $b << 56);
    $i++;
    $offset = $i;
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

function unpack_i64_dyn_v2($str, &$offset = 0)
{
    $v = unpack_u64_dyn_v2($str, $offset);
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
