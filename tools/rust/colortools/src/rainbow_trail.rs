use crate::{color_utils::*, CCColor3BR};

#[allow(non_snake_case)]
#[unsafe(no_mangle)]
pub extern "C" fn get_rainbow(offset: f32, saturation: f32) -> CCColor3BR {
    let mut R = 0.0;
    let mut G = 0.0;
    let mut B = 0.0;
    let mut hue = (unsafe { OWO } + offset) % 360.0;
    let mut sat = saturation / 100.0;

    hsv_to_rgb(&mut R, &mut G, &mut B, &mut hue, &mut sat, &mut 1.0);

    CCColor3BR {
        r: (R * 255.0) as u8,
        g: (G * 255.0) as u8,
        b: (B * 255.0) as u8,
    }
}

#[no_mangle]
pub extern "C" fn get_gradient(
    phase: f32,
    offset: f32,
    smooth: bool,
    colors: *const CCColor3BR,
    colors_len: usize,
) -> CCColor3BR {
    let mut out = CCColor3BR { r: 0, g: 0, b: 0 };

    if !colors.is_null() && colors_len > 0 {
        let colors_slice = unsafe { std::slice::from_raw_parts(colors, colors_len) };

        let phase = (phase + offset) % 360.0;
        let t = phase / 360.0;
        let mut y = t;

        if smooth {
            y = (1.0 - y * 2.0).abs();
        }

        let index = ((t * colors_len as f32) % colors_len as f32).floor() as usize;
        let next_index = (index + 1) % colors_len;

        out.r = colors_slice[index].r + (colors_slice[next_index].r - colors_slice[index].r);
        out.g = colors_slice[index].g + (colors_slice[next_index].g - colors_slice[index].g);
        out.b = colors_slice[index].b + (colors_slice[next_index].b - colors_slice[index].b);
    }

    out
}
