use core::fmt;
use core::fmt::Write;

pub fn println_formatted(fmt: fmt::Arguments) {
    unsafe {
        writeln!(::SP.as_mut().unwrap(), "{}", fmt);
    }
}

extern {
    pub fn inb(port: u16) -> u8;
    pub fn outb(port: u16, datum: u8);
}
