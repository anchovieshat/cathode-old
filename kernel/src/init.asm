bits 64

extern main
global _start
global inb
global memcpy

section .text

_start:
	call main
.loop:
	cli
	hlt
	jmp .loop

inb:
	xor rax, rax
	mov dx, di
	in al, dx
	ret

memcpy:
	mov rcx, rdx
	rep movsb
	ret
