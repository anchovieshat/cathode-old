use core::ptr::PtrExt;

/*pub fn outb(port: u16, datum: u8) {
    unsafe {
        asm!("outb %0, %1"
             :
             : "a"(datum), "Nd"(port));
    }
}*/

/*pub fn inb(port: u16) -> u8 {
    unsafe {
        let mut ret: u8 = mem::uninitialized();
        asm!("inb %1, %0"
             : "=a"(ret)
             : "Nd"(port));
        return ret;
    }
}*/

extern {
    pub fn inb(port: u16) -> u8;
    pub fn outb(port: u16, datum: u8);
}
