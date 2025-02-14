use crate::{hsv_to_rgb, CCColor3BR, OWO};
use std::collections::HashMap;

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
        b: (B * 255.0) as u8
    }
}

#[unsafe(no_mangle)]
#[allow(unused_assignments)]
pub extern "C" fn get_gradient(phase: f32, offset: f32, smooth: bool, colors: &HashMap<i32, CCColor3BR>) -> CCColor3BR {
    let mut out = CCColor3BR { r: 0, g: 0, b: 0 };
    
    if !colors.is_empty() {
        let phase = (phase + offset) % 360.0;
        let t = phase / 360.0;
        let mut y = t;
        
        if smooth {
            y = (1.0 - y * 2.0).abs();
        }
        
        let index = ((t * colors.len() as f32) % colors.len() as f32).floor() as i32;
        let next_index = (index + 1) % colors.len() as i32;
        
        out.r = colors[&index].r + (colors[&next_index].r - colors[&index].r);
        out.g = colors[&index].g + (colors[&next_index].g - colors[&index].g);
        out.b = colors[&index].b + (colors[&next_index].b - colors[&index].b);
    }
    
    out
}