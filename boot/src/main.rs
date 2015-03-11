#![feature(no_std,asm,lang_items,core,unicode,unique)]
#![crate_name="boot"]
#![crate_type="staticlib"]
#![no_std]

#[macro_use]
extern crate core;
extern crate unicode;

macro_rules! println {
    ($($arg:tt)*) => (::println_formatted(format_args!($($arg)*)))
}

mod reloc;
mod lang;
mod efi;

use core::prelude::*;
use core::ptr;
use core::fmt;
use core::fmt::Write;
pub use reloc::_reloc;

static mut ST: Option<ptr::Unique<efi::SystemTable>> = None;
static mut CONSOLE: Option<efi::Console> = None;

fn println_formatted(args: fmt::Arguments) {
    unsafe {
        writeln!(CONSOLE.as_mut().unwrap(), "{}", args);
    }
}

#[no_mangle]
pub fn start(image_handle: efi::Handle, sys_table: *mut efi::SystemTable) {
    unsafe {
        ST = Some(ptr::Unique::new(sys_table));
    }
    let c = unsafe { efi::Console::new(ST.as_ref().unwrap().get().con_out) };
    unsafe {
        CONSOLE = Some(c);
    }
    println!("Handle: {:x}", image_handle as usize);
    panic!("Failed to start kernel");
}
