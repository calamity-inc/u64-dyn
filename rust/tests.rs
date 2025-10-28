use u64_dyn::*;

#[test]
fn test_u64() {
    let cases = [
        (0u64, vec![0x00]),
        (0x7f, vec![0x7f]),
        (0x80, vec![0x80, 0x01]),
        (1337, vec![0xb9, 0x0a]),
        (42069, vec![0xd5, 0xc8, 0x02]),
        (0xffffffffffffffff, vec![0xff; 9]),
        (0x8000000000000000, vec![0x80; 9]),
    ];
    for (val, enc) in cases {
        assert_eq!(pack_u64_dyn(val), enc);
        assert_eq!(unpack_u64_dyn(&enc), Some((val, enc.len())));
    }
}

#[test]
fn test_u64_v2() {
    let cases = [
        (0u64, vec![0x00]),
        (0x7f, vec![0x7f]),
        (0x80, vec![0x80, 0x00]),
        (1337, vec![0xb9, 0x09]),
        (42069, vec![0xd5, 0xc7, 0x01]),
        (0x123456789abcdef, vec![0xef, 0x9a, 0xae, 0xcc, 0xf7, 0xab, 0xd0, 0x90, 0x00]),
        (0xffffffffffffffff, vec![0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe]),
        (0x8000000000000000, vec![0x80, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x7e]),
    ];
    for (val, enc) in cases {
        assert_eq!(pack_u64_dyn_b(val), enc);
        assert_eq!(unpack_u64_dyn_b(&enc), Some((val, enc.len())));
    }
}

#[test]
fn test_i64() {
    let cases = [
        (0i64, vec![0x00]),
        (0x7f, vec![0xbf, 0x01]),
        (0x80, vec![0x80, 0x02]),
        (1337, vec![0xb9, 0x14]),
        (42069, vec![0x95, 0x91, 0x05]),
        (-1, vec![0x41]),
        (i64::MIN, vec![0x40]),
    ];
    for (val, enc) in cases {
        assert_eq!(pack_i64_dyn_a(val), enc);
        assert_eq!(unpack_i64_dyn_a(&enc), Some((val, enc.len())));
    }
}

#[test]
fn test_i64_v2() {
    let cases = [
        (0i64, vec![0x00]),
        (0x7f, vec![0xbf, 0x00]),
        (0x80, vec![0x80, 0x01]),
        (1337, vec![0xb9, 0x13]),
        (42069, vec![0x95, 0x90, 0x04]),
        (-1, vec![0x40]),
        (i64::MIN, vec![0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe]),
    ];
    for (val, enc) in cases {
        assert_eq!(pack_i64_dyn_b(val), enc);
        assert_eq!(unpack_i64_dyn_b(&enc), Some((val, enc.len())));
    }
}

#[test]
fn test_truncated() {
    let truncated = [
        vec![],
        vec![0x80],
        vec![0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80],
    ];
    for enc in truncated {
        assert!(unpack_u64_dyn(&enc).is_none());
        assert!(unpack_i64_dyn_a(&enc).is_none());
        assert!(unpack_i64_dyn_b(&enc).is_none());
        assert!(unpack_u64_dyn_b(&enc).is_none());
    }
}

#[test]
fn test_wrap_mod_64() {
    let enc = vec![0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe];
    assert_eq!(unpack_u64_dyn_b(&enc), Some((0x7f, 9)));
    assert_eq!(unpack_i64_dyn_b(&enc), Some((-64, 9)));
}

#[test]
#[allow(deprecated)]
fn test_deprecated_aliases() {
    let canonical_u = pack_u64_dyn_b(42069);
    assert_eq!(canonical_u, pack_u64_dyn_v2(42069));
    assert_eq!(
        unpack_u64_dyn_v2(&canonical_u),
        Some((42069, canonical_u.len()))
    );

    let canonical_a = pack_i64_dyn_a(-123);
    assert_eq!(canonical_a, pack_i64_dyn(-123));
    assert_eq!(
        unpack_i64_dyn(&canonical_a),
        Some((-123, canonical_a.len()))
    );

    let canonical_b = pack_i64_dyn_b(-456);
    assert_eq!(canonical_b, pack_i64_dyn_v2(-456));
    assert_eq!(
        unpack_i64_dyn_v2(&canonical_b),
        Some((-456, canonical_b.len()))
    );
}
