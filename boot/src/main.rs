#![feature(no_std,asm,lang_items,core,unicode)]
#![crate_name="boot"]
#![crate_type="staticlib"]
#![no_std]

extern crate core;
extern crate unicode;

mod lang;

use core::prelude::*;
use core::mem;

const DT_NULL: i64 = 0;
const DT_RELA: i64 = 7;
const DT_RELASZ: i64 = 8;
const DT_RELAENT: i64 = 9;
const DT_REL: i64 = 17;
const DT_RELSZ: i64 = 18;
const DT_RELENT: i64 = 19;

const R_RELATIVE: u64 = 8;

pub struct ElfDyn {
    d_tag: i64,
    d_val: u64,
}

pub struct ElfRel {
    r_offset: u64,
    r_info: u64,
}

type EfiTextReset = *const ();
type EfiTextString = extern "win64" fn (*const EfiSimpleTextOutputProtocol, *const u16);

#[repr(C)]
struct EfiSimpleTextOutputProtocol {
    reset: EfiTextReset,
    output_string: EfiTextString,
}

type EfiSimpleTextInputProtocol = ();

#[repr(C)]
struct EfiTableHeader {
	signature: u64,
	revision: u32,
	header_size: u32,
	crc32: u32,
	_reserved: u32,
}

#[repr(C)]
pub struct EfiSystemTable {
    hdr: EfiTableHeader,
    fw_vendor: *const u16,
    fw_revision: u32,
    console_in_handle: EfiHandle,
    con_in: *const EfiSimpleTextInputProtocol,
    con_out_handle: EfiHandle,
    con_out: *const EfiSimpleTextOutputProtocol,
}

pub type EfiHandle = *const ();

#[no_mangle]
pub unsafe extern fn _reloc(image_base: u64, dyn: *const ElfDyn, image_handle: EfiHandle, system_table: *const EfiSystemTable) {
    let mut relsz = 0u64;
    let mut relent = 0u64;

    let mut dynp: *const ElfDyn = dyn;
    let mut rel: *const ElfRel = mem::uninitialized();

    while (*dynp).d_tag != DT_NULL {
        match (*dynp).d_tag {
            DT_REL | DT_RELA => rel = mem::transmute((*dynp).d_val + image_base),
            DT_RELSZ | DT_RELASZ => relsz = (*dynp).d_val,
            DT_RELENT | DT_RELAENT => relent = (*dynp).d_val,
            _ => (),
        }
        dynp = dynp.offset(1);
    }

    while relsz > 0 {
        match (*rel).r_info & 0xffffffff {
            R_RELATIVE => {
                let newaddr: *mut u64 = mem::transmute(image_base + (*rel).r_offset);
                *newaddr += image_base;
            }
            _ => (),
        }
        relsz -= relent;
    }
}

#[no_mangle]
pub fn start(image_handle: EfiHandle, sys_table: *const EfiSystemTable) {
    unsafe {
        let con_out = (*sys_table).con_out;
        for c in unicode::str::Utf16Encoder::new("Hello World".chars()) {
            let strarr: [u16; 2] = [c, 0];
            ((*con_out).output_string)(con_out, strarr.as_ptr());
        }
    }
    loop { }
}
