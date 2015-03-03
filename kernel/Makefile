RUSTC ?= rustc
AS ?= yasm
ASFLAGS := -felf64
TARGET := kernel
LD ?= ld

.SUFFIXES:
.PHONY: all
.PRECIOUS: %/.dir
.DEFAULT: all
.SECONDEXPANSION:

OBJ_FILES := kernel.o init.o

OBJS := $(addprefix build/,$(OBJ_FILES))
DEPS := $(addprefix build/,$(OBJ_FILES:.o=.d))

all: $(TARGET)

$(TARGET): $(OBJS)
	@$(LD) -O1 -nostdlib -Bsymbolic -z defs -shared -T kernel.ld -o $@ $^ ../lib/libcore.rlib

build/kernel.o: $$(@D)/.dir
	@echo "RUSTC $(TARGET)"
	@RUST_TARGET_PATH=../lib $(RUSTC) -C lto -O -L ../lib --emit obj,dep-info --out-dir build --target x86_64-unknown-kernel src/main.rs
	@objcopy -R .note.GNU-stack $@ $@

kernel.d: kernel.o

build/%.o: src/%.asm $$(@D)/.dir
	@echo "AS $(@F)"
	@$(AS) $(ASFLAGS) -o $@ $<

%/.dir:
	@echo "MKDIR $(@D)"
	@mkdir -p $(@D)
	@touch $@

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
