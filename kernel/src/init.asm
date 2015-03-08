bits 64

extern main
global _start

section .boot

_start:
	call main
.loop:
	cli
	hlt
	jmp .loop
