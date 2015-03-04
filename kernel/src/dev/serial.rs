use core::fmt;
use core::str::StrExt;
use core::result::Result;

pub struct SerialPort {
    base_port: u16
}

impl SerialPort {
    pub fn init(port: u16) -> SerialPort {

        SerialPort {
            base_port: port,
        }
    }

    fn buffer_ready(&mut self) -> bool {
        unsafe {
            !((::io::inb(self.base_port+5) & 0x20) == 0)
        }
    }

    pub fn putc(&mut self, c: char) {
        while !self.buffer_ready() { }
        unsafe {
            ::io::outb(self.base_port, c as u8);
        }
    }
}

impl fmt::Write for SerialPort {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        for c in s.chars() {
            self.putc(c);
        }
        Result::Ok(())
    }
}
