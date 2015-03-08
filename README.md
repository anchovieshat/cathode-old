# Cathode

## Building Cathode on Linux

1. Install all dependencies:
	* `rustc`
	* `yasm`
	* `clang`
	* `ld.gold`
	* `objcopy`
	* `dd`
	* `parted`
	* `mtools`
	* `ninja`
	* `ovmf`
	* `qemu`

2. Clone Rust [source]
[source]: https://github.com/rust-lang/rust

3. Build and Install:
	```sh
	$ ./configure /path/to/rust/source
	$ ninja disk
	$ qemu-system-x86_64 -bios /usr/share/ovmf/ovmf_x86.bin -m 512 -serial stdio build/disk.img
	```

Currently tested on Arch Linux
