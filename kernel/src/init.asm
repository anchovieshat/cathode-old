bits 64

extern main
global _start

section .boot

_start:
	mov rsp, rsi
	call load_gdt
	call main
.loop:
	cli
	hlt
	jmp .loop

default rel

section .text

load_gdt:
	lea rax, [gdt.null_seg]
	mov [gdt.gdtloc], rax
	lgdt [gdt]
	lea rax, [.newcs]
	mov [.offset], rax
	mov dword [.selector], 0x08
	jmp [.offset]
	.offset:		dq 0
	.selector: 	dd 0
	nop
	nop
.newcs:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	ret

section .data

gdt:
	dw .end - .null_seg
.gdtloc:	dd 0
.null_seg:
	dq 0x0000000000000000
.code_seg:
	dq 0x0020980000000000
.data_seg:
	dq 0x0000920000000000
.end:
