
#[lang="eh_personality"]
fn eh_personality() { }

#[lang="panic_fmt"]
fn panic_fmt() -> ! { loop { } }

#[lang="stack_exhausted"]
fn stack_exhausted() -> ! { loop { } }