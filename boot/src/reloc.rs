use core::prelude::*;
use core::mem;
use efi;

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


#[no_mangle]
#[allow(unused_variables)]
pub unsafe extern fn _reloc(image_base: u64, dyn: *const ElfDyn, image_handle: efi::Handle, system_table: *const efi::SystemTable) {
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
        let newrel = mem::transmute(rel) + relent;
        rel = mem::transmute(newrel);
        relsz -= relent;
    }
}

