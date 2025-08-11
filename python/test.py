import unittest
from u64_dyn import (
    pack_u64_dyn, unpack_u64_dyn,
    pack_u64_dyn_v2, unpack_u64_dyn_v2,
    pack_i64_dyn, unpack_i64_dyn,
    pack_i64_dyn_v2, unpack_i64_dyn_v2,
)


class U64DynTests(unittest.TestCase):
    def test_pack_unpack_u64(self):
        cases = {
            0: bytes([0x00]),
            0x7F: bytes([0x7F]),
            0x80: bytes([0x80, 0x01]),
            1337: bytes([0xB9, 0x0A]),
            42069: bytes([0xD5, 0xC8, 0x02]),
            0xFFFFFFFFFFFFFFFF: bytes([0xFF] * 9),
            0x8000000000000000: bytes([0x80] * 9),
        }
        for val, enc in cases.items():
            self.assertEqual(pack_u64_dyn(val), enc)
            value, off = unpack_u64_dyn(enc)
            self.assertEqual(value, val)
            self.assertEqual(off, len(enc))

    def test_pack_unpack_i64(self):
        cases = {
            0: bytes([0x00]),
            0x7F: bytes([0xBF, 0x01]),
            0x80: bytes([0x80, 0x02]),
            1337: bytes([0xB9, 0x14]),
            42069: bytes([0x95, 0x91, 0x05]),
            -1: bytes([0x41]),
            -0x8000000000000000: bytes([0x40]),
        }
        for val, enc in cases.items():
            self.assertEqual(pack_i64_dyn(val), enc)
            value, off = unpack_i64_dyn(enc)
            self.assertEqual(value, val)
            self.assertEqual(off, len(enc))

    def test_pack_unpack_u64_v2(self):
        cases = {
            0: bytes([0x00]),
            0x7F: bytes([0x7F]),
            0x80: bytes([0x80, 0x00]),
            1337: bytes([0xB9, 0x09]),
            42069: bytes([0xD5, 0xC7, 0x01]),
            0xFFFFFFFFFFFFFFFF: bytes([0xFF, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE]),
            0x8000000000000000: bytes([0x80, 0xFF, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0x7E]),
        }
        for val, enc in cases.items():
            self.assertEqual(pack_u64_dyn_v2(val), enc)
            value, off = unpack_u64_dyn_v2(enc)
            self.assertEqual(value, val)
            self.assertEqual(off, len(enc))

    def test_pack_unpack_i64_v2(self):
        cases = {
            0: bytes([0x00]),
            0x7F: bytes([0xBF, 0x00]),
            0x80: bytes([0x80, 0x01]),
            1337: bytes([0xB9, 0x13]),
            42069: bytes([0x95, 0x90, 0x04]),
            -1: bytes([0x40]),
            -0x8000000000000000: bytes([0xFF, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE]),
        }
        for val, enc in cases.items():
            self.assertEqual(pack_i64_dyn_v2(val), enc)
            value, off = unpack_i64_dyn_v2(enc)
            self.assertEqual(value, val)
            self.assertEqual(off, len(enc))

    def test_truncated(self):
        truncated = [
            b"",
            bytes([0x80]),
            bytes([0x80] * 8),
        ]
        for enc in truncated:
            with self.assertRaises(ValueError):
                unpack_u64_dyn(enc)
            with self.assertRaises(ValueError):
                unpack_i64_dyn(enc)
            with self.assertRaises(ValueError):
                unpack_u64_dyn_v2(enc)
            with self.assertRaises(ValueError):
                unpack_i64_dyn_v2(enc)


if __name__ == '__main__':
    unittest.main()
