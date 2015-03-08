#![feature(asm,no_std,lang_items,core)]
#![no_std]
#![crate_name="kernel"]

#![allow(unused_must_use)]

#[macro_use]
extern crate core;

use core::prelude::*;
use core::fmt::Write;

macro_rules! println {
    ($($arg:tt)*) => (::io::println_formatted(format_args!($($arg)*)))
}

mod lang;
mod io;
mod dev;
mod mem;
mod gdt;

#[allow(dead_code)]
#[repr(u32)]
#[derive(Debug)]
#[derive(PartialEq)]
#[derive(Eq)]
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
    kernel_base: u64,
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

pub static mut SP: Option<dev::serial::SerialPort> = None;

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
    unsafe {
        let sp = dev::serial::SerialPort::init(0x3F8);
        SP = Some(sp);
    }

    gdt::init();

    println!("KERNEL START");

    let mut max_rg_sz = 0usize;
    let mut max_rg_ptr = 0usize;

    unsafe {
        let bpi = BootProtoIter { map: (*bootproto).mem_map, size: (*bootproto).map_size, ent_size: (*bootproto).map_ent_size, idx: 0};

        for region in bpi {
            println!("{:?} from {:x} -> {:x} (sz {:x})", (*region).type_, (*region).phys_start, (*region).phys_start+((*region).npages*4096), (*region).npages*4096);
            if (*region).type_ == EfiMemoryType::EfiConventionalMemory && ((*region).npages*4096) as usize > max_rg_sz {
                max_rg_sz = ((*region).npages*4096) as usize;
                max_rg_ptr = (*region).phys_start as usize;
            }
        }
    }

    let maxmem = unsafe {
        let bpi = BootProtoIter { map: (*bootproto).mem_map, size: (*bootproto).map_size, ent_size: (*bootproto).map_ent_size, idx:0 };
        let last = bpi.last().unwrap();
        ((*last).phys_start+((*last).npages*4096)) as usize
    };

    println!("maxmem is {:x} ({} MB)", maxmem, maxmem/1024/1024);

    let mut heap = mem::Heap::new(maxmem, max_rg_ptr, max_rg_sz);
    let mut pg5 = 0;
    println!("Heap testing: ");
    for i in (0..15) {
        println!("Allocating span of 1024 pages (4M)...");
        match heap.alloc_pages(1024) {
            Some(page) => {
                println!("...success, allocated page {:x} at {:x}", page, page << 12);
                if i == 5 {
                    pg5 = page;
                };
            },
            None => {
                println!("...failed (out of memory?)");
            }
        };
    }

    println!("Deallocating allocation 5 (page {:x} at {:x})...", pg5, pg5 << 12);

    heap.clear_used_range(pg5, 1024);

    println!("Allocating a 2048 (8M) span (order 11)...");

    match heap.alloc_pages(2048) {
        Some(page) => println!("...success, allocated page {:x} at {:x}", page, page << 12),
        None => println!("...failed."),
    };

    println!("Allocating one more 1024 (4M) span (order 10)...");

    match heap.alloc_pages(1024) {
        Some(page) => println!("...success, allocated page {:x} at {:x}", page, page << 12),
        None => println!("...failed."),
    };

    panic!("Reached end of main");
}
