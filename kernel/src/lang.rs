use core::fmt;

#[lang="eh_personality"]
pub fn eh_personality() { }

#[lang="panic_fmt"]
extern fn panic_fmt(fmt: fmt::Arguments, file: &'static str, line: usize) -> ! {
    println!("panic: {} ({}:{})", fmt, file, line);
    loop {
        unsafe {
            asm!("hlt");
        }
    }
}

#[lang="stack_exhausted"]
fn stack_exhausted() -> ! { loop { } }
