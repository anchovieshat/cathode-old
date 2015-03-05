#![feature(asm,no_std,lang_items,core)]
#![no_std]
#![crate_name="kernel"]

#![allow(unused_must_use)]

#[macro_use]
extern crate core;

use core::prelude::*;
use core::fmt::Write;

mod lang;
mod io;
mod dev;

#[allow(dead_code)]
#[repr(u32)]
#[derive(Debug)]
enum EfiMemoryType {
	EfiReservedMemoryType,
	EfiLoaderCode,
	EfiLoaderData,
	EfiBootServicesCode,
	EfiBootServicesData,
	EfiRuntimeServicesCode,
	EfiRuntimeServicesData,
	EfiConventionalMemory,
	EfiUnusableMemory,
	EfiACPIReclaimMemory,
	EfiACPIMemoryNVS,
	EfiMemoryMappedIO,
	EfiMemoryMappedIOPortSpace,
	EfiPalCode,
	EfiMaxMemoryType,
}

#[repr(C)]
struct EfiMemoryDescriptor {
    type_: EfiMemoryType,
    phys_start: u64,
    virt_start: u64,
    npages: u64,
    attrs: u64,
}

#[repr(C)]
pub struct BootProto {
    mem_map: *const EfiMemoryDescriptor,
    map_size: u64,
    map_ent_size: u64
}

struct BootProtoIter {
    map: *const EfiMemoryDescriptor,
    size: u64,
    ent_size: u64,
    idx: u64,
}

impl Iterator for BootProtoIter {
    type Item = *const EfiMemoryDescriptor;
    fn next(&mut self) -> Option<*const EfiMemoryDescriptor> {
        if self.idx * self.ent_size >= self.size {
            None
        } else {
            let r = Some((self.map as u64 + (self.idx*self.ent_size)) as *const EfiMemoryDescriptor);
            self.idx += 1;
            r
        }
    }
}

#[no_mangle]
pub fn main(bootproto: *const BootProto) {
    let mut sp = dev::serial::SerialPort::init(0x3F8);

    writeln!(&mut sp, "KERNEL START");

    unsafe {
        let bpi = BootProtoIter { map: (*bootproto).mem_map, size: (*bootproto).map_size, ent_size: (*bootproto).map_ent_size, idx: 0};

        for region in bpi {
            write!(&mut sp, "{:?} from {:x} -> {:x}\n", (*region).type_, (*region).phys_start, (*region).phys_start+((*region).npages*4096));
        }
    }

    write!(&mut sp, "\n\nReached end of main - HALT");
    loop {
        unsafe {
            asm!("hlt");
        }
    }
}
