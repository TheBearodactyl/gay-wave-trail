mod rainbow_trail;

use std::ffi::CStr;

#[repr(C)]
pub struct CCColor3BR {
    r: u8,
    g: u8,
    b: u8,
}

#[unsafe(no_mangle)]
pub static mut OWO: f32 = 0.0;

#[unsafe(no_mangle)]
pub extern "C" fn hsv_to_rgb(
    r: &mut f32,
    g: &mut f32,
    b: &mut f32,
    h: &mut f32,
    s: &mut f32,
    v: &mut f32,
) {
    let c = *v * *s;
    let x = c * (1.0 - ((*h / 60.0) % 2.0 - 1.0).abs());
    let m = *v - c;

    *r = if *h < 60.0 {
        c
    } else if *h < 120.0 {
        x
    } else if *h < 180.0 {
        0.0
    } else if *h < 240.0 {
        0.0
    } else if *h < 300.0 {
        x
    } else {
        c
    };

    *g = if *h < 60.0 {
        x
    } else if *h < 120.0 {
        c
    } else if *h < 180.0 {
        c
    } else if *h < 240.0 {
        x
    } else if *h < 300.0 {
        0.0
    } else {
        0.0
    };

    *b = if *h < 60.0 {
        0.0
    } else if *h < 120.0 {
        0.0
    } else if *h < 180.0 {
        x
    } else if *h < 240.0 {
        c
    } else if *h < 300.0 {
        c
    } else {
        x
    };

    *r += m;
    *g += m;
    *b += m;
}

#[unsafe(no_mangle)]
pub extern "C" fn hex_to_hsv(hex: u32, h: &mut f32, s: &mut f32, v: &mut f32) {
    let r = ((hex >> 16) & 0xFF) as f32 / 255.0;
    let g = ((hex >> 8) & 0xFF) as f32 / 255.0;
    let b = (hex & 0xFF) as f32 / 255.0;

    let max = r.max(g).max(b);
    let min = r.min(g).min(b);
    *v = max;

    let delta = max - min;
    if max != 0.0 {
        *s = delta / max;
    } else {
        *s = 0.0;
        *h = 0.0;
        return;
    }

    *h = if r == max {
        (g - b) / delta
    } else if g == max {
        2.0 + (b - r) / delta
    } else {
        4.0 + (r - g) / delta
    };

    *h *= 60.0;
    if *h < 0.0 {
        *h += 360.0;
    }
}

#[unsafe(no_mangle)]
pub extern "C" fn hex_to_rgb(hex: *const std::os::raw::c_char) -> CCColor3BR {
    let hex_str = unsafe { CStr::from_ptr(hex).to_string_lossy() };

    if !is_valid_hex_code(hex) {
        return CCColor3BR { r: 0, g: 0, b: 0 };
    }

    let clean_hex = if hex_str.starts_with('#') {
        &hex_str[1..]
    } else {
        &hex_str
    };

    let r = hex_pair_to_dec(clean_hex[0..2].as_ptr() as *const std::os::raw::c_char);
    let g = hex_pair_to_dec(clean_hex[2..4].as_ptr() as *const std::os::raw::c_char);
    let b = hex_pair_to_dec(clean_hex[4..6].as_ptr() as *const std::os::raw::c_char);

    CCColor3BR { r, g, b }
}

#[unsafe(no_mangle)]
pub extern "C" fn hex_pair_to_dec(hex_pair: *const std::os::raw::c_char) -> u8 {
    let hex_str = unsafe { CStr::from_ptr(hex_pair).to_string_lossy() };

    let mut val = 0;

    for c in hex_str.chars() {
        val = val * 16;
        val += if c.is_ascii_digit() {
            c as i32 - '0' as i32
        } else {
            c.to_ascii_lowercase() as i32 - 'a' as i32 + 10
        };
    }

    val as u8
}

#[unsafe(no_mangle)]
pub extern "C" fn is_valid_hex_code(hex_code: *const std::os::raw::c_char) -> bool {
    let hex_str = unsafe { CStr::from_ptr(hex_code).to_string_lossy() };

    if hex_str.is_empty() {
        return false;
    }

    let start_pos = if hex_str.starts_with("#") { 0 } else { 1 };

    if hex_str.len() != start_pos + 6 {
        return false;
    }

    hex_str[start_pos..].chars().all(|c| c.is_ascii_hexdigit())
}

