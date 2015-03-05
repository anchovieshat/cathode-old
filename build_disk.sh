#!/bin/sh

BOOT_SECTORS=262144
DISK_SECTORS=266240
DISK_START=2048
DISK_END=264192

PARTED=parted
PARTED_PARAMS="-s -a minimal"

build_disk() {
	dd if=/dev/zero of=$1 bs=512 count=$DISK_SECTORS >/dev/null 2>&1
	$PARTED $@ $PARTED_PARAMS mklabel gpt
	$PARTED $@ $PARTED_PARAMS mkpart EFI FAT32 ${DISK_START}s ${DISK_END}s
	$PARTED $@ $PARTED_PARAMS toggle 1 boot
	dd if=boot.img of=$1 bs=512 obs=512 count=$BOOT_SECTORS seek=$DISK_START conv=notrunc >/dev/null 2>&1
}

build_efi_partition() {
	dd if=/dev/zero of=$1 bs=512 count=$BOOT_SECTORS >/dev/null 2>&1
	mkfs.vfat -F32 $1 >/dev/null
	mmd -i $1 ::/EFI
	mmd -i $1 ::/EFI/Boot
	mcopy -i $1 boot/boot.efi ::/EFI/Boot/bootx64.efi
	mcopy -i $1 kernel/kernel ::/kernel.elf
}

cd build

build_efi_partition 'boot.img' || exit 1
build_disk 'disk.img' || exit 1
