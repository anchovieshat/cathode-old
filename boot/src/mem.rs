use efi;
use efi::AllocateType::*;
use efi::MemoryType::*;
use core;
use core::mem;
use core::cmp::min;
use core::ptr;
use core::prelude::*;

const PAGE_SHIFT: usize = 12;
const PAGESZ: usize = (1 << PAGE_SHIFT);

#[inline(always)]
fn sz_to_pg(sz: usize) -> usize {
    (sz + (PAGESZ - 1)) >> PAGE_SHIFT
}

#[no_mangle]
pub unsafe extern fn rust_allocate(size: usize, align: usize) -> *mut u8 {
    println!("Allocate: {:x}, {:x}", size, align);
    if size == 0 {
        return ptr::null_mut();
    }
    if align > PAGESZ {
        panic!("Can't allocate memory pool on alignment {}", align);
    }
    let mut mem: usize = mem::uninitialized();
    let st = ((*::ST.as_ref().unwrap().get().boot_services).allocate_pages)(AllocateAnyPages, EfiLoaderData, sz_to_pg(size), &mut mem);
    if st != 0 {
        return ptr::null_mut();
    }
    println!("Return {:x}", mem);
    mem::transmute(mem)
}

#[no_mangle]
pub unsafe extern fn rust_deallocate(ptr: *mut u8, size: usize, align: usize) {
    if ptr.is_null() { return; }
    ((*::ST.as_ref().unwrap().get().boot_services).free_pages)(ptr as usize, sz_to_pg(size));
}

#[no_mangle]
pub unsafe extern fn rust_reallocate(ptr: *mut u8, old_size: usize, size: usize, align: usize) -> *mut u8 {
    println!("Reallocate: {:x}, {:x}, {:x}", ptr as usize, size, align);
    if align > PAGESZ {
        panic!("Can't reallocate memory pool on alignment {}", align);
    }
    let mut mem: usize = mem::uninitialized();
    let st = ((*::ST.as_ref().unwrap().get().boot_services).allocate_pages)(AllocateAnyPages, EfiLoaderData, sz_to_pg(size),  &mut mem);
    if st != 0 {
        return ptr::null_mut();
    }
    let memaddr: *mut u8 = mem::transmute(mem);
    println!("doing the copy (to {:x})", mem);
    core::intrinsics::copy_nonoverlapping_memory(memaddr, ptr, min(old_size, size));
    ((*::ST.as_ref().unwrap().get().boot_services).free_pages)(ptr as usize, sz_to_pg(old_size));
    println!("Return {:x}", memaddr as usize);
    memaddr
}

#[no_mangle]
pub unsafe extern fn rust_reallocate_inplace(ptr: *mut u8, old_size: usize, size: usize, align: usize) -> usize {
    rust_usable_size(old_size, align)
}

#[no_mangle]
pub unsafe extern fn rust_usable_size(size: usize, align: usize) -> usize {
    PAGESZ * sz_to_pg(size)
}

#[no_mangle]
pub unsafe extern fn rust_stats_print() {
    println!("Not having stats is so much cooler than having stats!");
}
