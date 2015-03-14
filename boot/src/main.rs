#![feature(no_std,asm,lang_items,core,unicode,unique)]
#![crate_name="boot"]
#![crate_type="staticlib"]
#![no_std]

#[macro_use]
extern crate core;
extern crate unicode;
extern crate alloc;
extern crate collections;

macro_rules! println {
    ($($arg:tt)*) => (::println_formatted(format_args!($($arg)*)))
}

mod reloc;
mod mem;
mod lang;
mod efi;

use core::prelude::*;
use core::ptr;
use core::fmt;
use core::fmt::Write;
pub use reloc::_reloc;
pub use mem::{rust_allocate, rust_deallocate, rust_reallocate, rust_reallocate_inplace, rust_usable_size, rust_stats_print};

static mut ST: Option<ptr::Unique<efi::SystemTable>> = None;
static mut CONSOLE: Option<efi::Console> = None;

fn println_formatted(args: fmt::Arguments) {
    unsafe {
        writeln!(CONSOLE.as_mut().unwrap(), "{}", args);
    }
}

#[no_mangle]
pub unsafe fn start(image_handle: efi::Handle, sys_table: *mut efi::SystemTable) {
    ST = Some(ptr::Unique::new(sys_table));
    CONSOLE = Some(efi::Console::new(ptr::Unique::new(ST.as_ref().unwrap().get().con_out)));
    let st = ST.as_ref().unwrap().get();
    let cons = CONSOLE.as_mut().unwrap();
    cons.clear();
    let lip: *const efi::LoadedImageProtocol = st.handle_protocol(image_handle, &efi::LOADED_IMAGE_PROTOCOL_GUID);
    println!("Loaded at: {:x}", (*lip).image_base as usize);
    let dip: *const efi::SimpleFileSystemProtocol = st.handle_protocol((*lip).device_handle, &efi::SIMPLE_FILE_SYSTEM_PROTOCOL_GUID);
    println!("dip returned {:x}", dip as usize);
    let mut root: *const efi::FileProtocol = unsafe { core::mem::uninitialized() };
    println!("open_volume: {}", ((*dip).open_volume)(dip, &mut root));
    println!("root filesystem @{:x}", root as usize);
    let mut kernel_file: *const efi::FileProtocol = unsafe { core::mem::uninitialized() };
    let mut kernel_file_name: collections::Vec<u16> = unicode::str::Utf16Encoder::new("\\kernel.elf".chars()).collect();
    kernel_file_name.push(0);
    println!("open: {}", ((*root).open)(root, &mut kernel_file, kernel_file_name.as_ptr(), efi::FileMode::Read, core::mem::transmute(0u64)));
    println!("kernel_file is {:x}", kernel_file as usize);
    let mut kernel_info_size: usize = 0;
    println!("(get_info is {:x})", ((*kernel_file).get_info) as usize);
    println!("get_info: {:x}", ((*kernel_file).get_info)(kernel_file, &efi::FILE_INFO_GUID, &mut kernel_info_size, ptr::null_mut()));
    println!("FileInfo buffer size is {}", kernel_info_size);
    let kernel_info: *mut efi::FileInfo = core::mem::transmute(rust_allocate(kernel_info_size, 1));
    println!("get_info: {:x}", ((*kernel_file).get_info)(kernel_file, &efi::FILE_INFO_GUID, &mut kernel_info_size, kernel_info));
    println!("Kernel file info: {:?}", *kernel_info);

    panic!("Failed to start kernel");
}
