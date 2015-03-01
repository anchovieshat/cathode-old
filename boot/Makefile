ifndef MAKE_RESTARTS
define Print
$(info ** $(1) **)
endef
endif

ifdef OPT
$(call Print,Optimization enabled)
OPTFLAG := -O3
else
OPTFLAG := -ggdb -O1
endif

CC := clang
AS := yasm
GAS := as
LD := ld
OBJCOPY := objcopy
PARTED := parted
CFLAGS := -pipe -Wall -Wextra -std=c11 -fPIC -ffreestanding $(OPTFLAG)
ASFLAGS := -felf64
LDFLAGS :=

ifdef DEBUG
CFLAGS := $(CFLAGS) -DDEBUG
endif

TARGET := boot.efi
OBJ_FILES := main.o reloc.o start.o

OBJS := $(addprefix build/,$(OBJ_FILES))
DEPS := $(addprefix deps/,$(OBJ_FILES:.o=.d))

.SUFFIXES:
.SECONDEXPANSION:
.PRECIOUS: %/.dir
.PHONY: all clean depend disk
.DEFAULT: all

all: $(TARGET)
depend: $(DEPS)

$(TARGET): $(OBJS) $(DIR)
	@echo "LD $@"
	@$(LD) -nostdlib -znocombreloc -Bsymbolic -shared -T efi.ld -o $@.tmp $(OBJS)
	@$(OBJCOPY) -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel.dyn -j .rela.dyn -j .reloc -j .eh_frame --output-target efi-app-x86_64 $@.tmp $@
	@rm $@.tmp

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

build/%.o: src/%.c deps/%.d $$(@D)/.dir
	@echo "CC $(@F)"
	@$(CC) $(CFLAGS) -c -o $@ $<

build/%.o: src/%.asm $$(@D)/.dir
	@echo "AS $(@F)"
	@$(AS) $(ASFLAGS) -o $@ $<

#build/%.o: src/%.S $$(@D)/.dir
#	@echo "AS $(@F)"
#	@$(GAS) $(GASFLAGS) -o $@ $<

deps/%.d: src/%.c $$(@D)/.dir
	@echo "DEP $(@F)"
	@$(CC) $(CFLAGS) -MM -MP -MT $(subst .c,.o,$(subst src/,build/,$<)) $< > $@

%/.dir:
	@echo "MKDIR $(@D)"
	@mkdir -p $(@D)
	@touch $@

clean:
	-rm -rf build deps $(TARGET)

disk: disk.img

disk.img: boot.img
	@dd if=/dev/zero of=$@ bs=512 count=$(DISK_SECTORS)
	@$(PARTED) $@ $(PARTED_PARAMS) mklabel gpt
	@$(PARTED) $@ $(PARTED_PARAMS) mkpart EFI FAT32 $(DISK_START)s $(DISK_END)s
	@$(PARTED) $@ $(PARTED_PARAMS) toggle 1 boot
	@dd if=boot.img of=$@ bs=512 obs=512 count=$(BOOT_SECTORS) seek=$(DISK_START) conv=notrunc

BOOT_SECTORS := 262144
DISK_SECTORS := 266240
DISK_START := 2048
DISK_END := 264192
boot.img: $(TARGET)
	@dd if=/dev/zero of=$@ bs=512 count=$(BOOT_SECTORS)
	@mkfs.vfat -F32 $@
	@mmd -i $@ ::/EFI
	@mmd -i $@ ::/EFI/Boot
	@mcopy -i $@ boot.efi ::/EFI/Boot/bootx64.efi
