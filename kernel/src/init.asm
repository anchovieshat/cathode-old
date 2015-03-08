bits 64

extern main
global _start

section .boot

_start:
	mov rsp, rsi
	call main
.loop:
	cli
	hlt
	jmp .loop
