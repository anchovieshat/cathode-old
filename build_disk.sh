#!/bin/sh

set -e

BOOT_SECTORS=65536
DISK_SECTORS=69631
DISK_START=2048
DISK_END=67584

PARTED=parted
PARTED_PARAMS="-s -a minimal"

build_disk() {
	dd if=/dev/zero of=$1 bs=512 count=$DISK_SECTORS >/dev/null 2>&1
	$PARTED $@ $PARTED_PARAMS mklabel gpt
	$PARTED $@ $PARTED_PARAMS mkpart EFI FAT32 ${DISK_START}s ${DISK_END}s
	$PARTED $@ $PARTED_PARAMS toggle 1 boot
}

build_efi_partition() {
	dd if=/dev/zero of=$1 bs=512 count=$BOOT_SECTORS >/dev/null 2>&1
	mformat -i $1 -h 32 -t 32 -n 64 -c 1 ::
	mmd -i $1 ::/EFI
	mmd -i $1 ::/EFI/Boot
}

copy_efi_files() {
	mcopy -n -D o -i $1 boot/boot.efi ::/EFI/Boot/bootx64.efi
	mcopy -n -D o -i $1 kernel/kernel ::/kernel.elf
}

copy_efi_partition() {
	dd if=$2 of=$1 bs=512 obs=512 count=$BOOT_SECTORS seek=$DISK_START conv=notrunc >/dev/null 2>&1
}

cd $1

[ ! -e boot.img ] && build_efi_partition 'boot.img'
copy_efi_files 'boot.img'
[ ! -e disk.img ] && build_disk 'disk.img'
copy_efi_partition 'disk.img' 'boot.img'
