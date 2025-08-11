<?php
require_once __DIR__ . '/u64_dyn.php';

$tests_u64 = [
    0 => "\x00",
    0x7f => "\x7F",
    0x80 => "\x80\x01",
    1337 => "\xB9\x0A",
    42069 => "\xD5\xC8\x02",
    -1 => "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
    -9223372036854775808 => "\x80\x80\x80\x80\x80\x80\x80\x80\x80",
];

foreach ($tests_u64 as $val => $enc)
{
    assert(pack_u64_dyn($val) == $enc);
    $offset = 0;
    assert(unpack_u64_dyn($enc, $offset) == $val);
    assert($offset == strlen($enc));
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
    $offset = 0;
    assert(unpack_u64_dyn_v2($enc, $offset) == $val);
    assert($offset == strlen($enc));
}

$tests_i64 = [
    0 => "\x00",
    0x7f => "\xBF\x01",
    0x80 => "\x80\x02",
    1337 => "\xB9\x14",
    42069 => "\x95\x91\x05",
    -1 => "\x41",
    PHP_INT_MIN => "\x40",
];

foreach ($tests_i64 as $val => $enc)
{
    assert(pack_i64_dyn($val) == $enc);
    $offset = 0;
    assert(unpack_i64_dyn($enc, $offset) == $val);
    assert($offset == strlen($enc));
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
    $offset = 0;
    assert(unpack_i64_dyn_v2($enc, $offset) == $val);
    assert($offset == strlen($enc));
}

$incomplete = [ "", "\x80", "\x80\x80\x80\x80\x80\x80\x80\x80" ];

foreach ($incomplete as $enc)
{
    try {
        $offset = 0;
        unpack_u64_dyn($enc, $offset);
        assert(false);
    } catch (Exception $e) {}

    try {
        $offset = 0;
        unpack_i64_dyn($enc, $offset);
        assert(false);
    } catch (Exception $e) {}

    try {
        $offset = 0;
        unpack_u64_dyn_v2($enc, $offset);
        assert(false);
    } catch (Exception $e) {}

    try {
        $offset = 0;
        unpack_i64_dyn_v2($enc, $offset);
        assert(false);
    } catch (Exception $e) {}
}

$enc = "\xFF\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE";
$offset = 0;
assert(unpack_u64_dyn_v2($enc, $offset) == 0x7F);
assert($offset == strlen($enc));
$offset = 0;
assert(unpack_i64_dyn_v2($enc, $offset) == -64);
assert($offset == strlen($enc));
