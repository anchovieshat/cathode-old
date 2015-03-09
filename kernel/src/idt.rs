#[repr(u8)]
enum Gate {
    Call = 0xc,
    Interrupt = 0xe,
    Trap = 0xf,
}

#[repr(packed)]
struct Descriptor {
    pub offset_low: u16,
    pub selector: u16,
    _zero: u8,
    pub type_attr: u8,
    pub offset_mid: u16,
    pub offset_high: u32,
    _zero2: u32,
}
