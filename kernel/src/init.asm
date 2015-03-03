bits 64

extern main
global _start
global inb
global memcpy

section .boot

_start:
	mov rax, 0x500000000
	mov qword [rax], 0xDEADBEEF
	call main
.loop:
	cli
	hlt
	jmp .loop

section .text

inb:
	xor rax, rax
	mov dx, di
	in al, dx
	ret

memcpy:
	mov rcx, rdx
	rep movsb
	ret
