.SUFFIXES:
.PHONY: all


all: libcore.stamp

libcore.stamp:
	@echo "RUSTC libcore"
	@RUST_TARGET_PATH=. rustc --target x86_64-unknown-kernel --crate-type rlib libcore/lib.rs
	@touch libcore.stamp
