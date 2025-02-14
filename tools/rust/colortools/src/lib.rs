#![allow(unused_assignments)]

mod rainbow_trail;
mod color_utils;

#[repr(C)]
#[derive(Clone)]
pub struct CCColor3BR {
    r: u8,
    g: u8,
    b: u8,
}