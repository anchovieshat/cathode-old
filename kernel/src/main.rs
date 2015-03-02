#![feature(asm,no_std,lang_items,core)]
#![no_std]
#![crate_name="kernel"]

#[macro_use]
extern crate core;

use core::fmt::Write;

mod lang;
mod io;
mod dev;

#[no_mangle]
pub fn main() {
    let mut sp = dev::serial::SerialPort::init(0x3F8);
    write!(&mut sp, "Hello world");
}
