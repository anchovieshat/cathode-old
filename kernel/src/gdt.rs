use core::prelude::*;

#[repr(u8)]
pub enum Ty {
    RO = 0,
    RO_A = 1,
    RW = 2,
    RW_A = 3,
    RO_S = 4,
    RO_SA = 5,
    RW_S = 6,
    RW_SA = 7,
    XO = 8,
    XO_A = 9,
    XR = 0xa,
    XR_A = 0xb,
    XO_C = 0xc,
    XO_CA = 0xd,
    XR_C = 0xe,
    XR_CA = 0xf,

}

#[repr(packed)]
pub struct Entry {
    limit_low: u16,
    base_low: u16,
    base_mid: u8,
    flags: u16,
    base_high: u8,
}

impl Entry {
    pub fn set_base(&mut self, addr: u32) {
        self.base_low = (addr & 0xFFFF) as u16;
        self.base_mid = ((addr >> 16) & 0xFF) as u8;
        self.base_high = ((addr >> 24) & 0xFF) as u8;
    }
    pub fn set_limit(&mut self, addr: u32) {
        self.limit_low = (addr & 0xFFFF) as u16;
        self.flags = (self.flags & !0xF00) | ((self.flags >> 8) & 0xF);
    }
    pub fn set_type(&mut self, ty: Ty) {
        let ty = ty as u16;
        self.flags = (self.flags & !0xFF) | ty;
    }
}

pub struct Table {
    entries: &'static [Entry],
}

impl Table {
    pub unsafe fn load(&mut self) {
        struct gdtr {
            limit: u16,
            addr: usize,
        }
        let x = gdtr { limit: self.entries.len() as u16, addr: self.entries.as_ptr() as usize };
        asm!("lgdt %0
              jmp 0x08:set_cs
             .set_cs:
              mov ax, 0x10
              mov ds, ax
              mov es, ax
              mov fs, ax
              mov gs, ax
              mov ss, ax"
              :
              : "p"(&x)
              : "ax"
              : "intel", "volatile");
    }
}

pub fn init() {
    static mut entries: [Entry; 3] = [
        Entry { limit_low: 0, base_low: 0, base_mid: 0, flags: 0, base_high: 0 },
        Entry { limit_low: 0, base_low: 0, base_mid: 0, flags: 0, base_high: 0 },
        Entry { limit_low: 0, base_low: 0, base_mid: 0, flags: 0, base_high: 0 },
    ];
    unsafe {
        let ref mut null = entries[0];
        let ref mut code = entries[1];
        let ref mut data = entries[2];
        code.set_base(0x0); code.set_limit(!0x0u32);
        data.set_base(0x0); data.set_limit(!0x0u32);
        code.set_type(Ty::XR);
        data.set_type(Ty::RW);
    }
    unsafe {
        static mut tbl: Table = Table {
            entries: &entries,
        };
        tbl.load();
    }
}
