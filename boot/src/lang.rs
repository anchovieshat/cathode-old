use core::prelude::*;
use core::fmt;
use core::fmt::Write;

#[lang="eh_personality"]
pub fn eh_personality() { }

#[lang="panic_fmt"]
extern fn panic_fmt(fmt: fmt::Arguments, file: &'static str, line: usize) -> ! {
    unsafe {
        match ::CONSOLE.as_mut() {
            Some(c) => writeln!(c, "panic: {} ({}:{})", fmt, file, line),
            None => Ok(()),
        };
    }
    unsafe {
        asm!("cli");
        loop {
            asm!("hlt");
        }
    }
}

#[lang="stack_exhausted"]
fn stack_exhausted() -> ! { loop { } }
