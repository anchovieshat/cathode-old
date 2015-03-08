bits 64

extern main
global _start
global inb
global outb
global memcpy
global memset

section .boot

_start:
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

outb:
	mov dx, di
	mov ax, si
	out dx, ax
	ret

memcpy:
	mov rcx, rdx
	rep movsb
	ret

memset:
	mov rax, rsi
	mov rcx, rdx
	rep stosb

	mov rax, rdi

	ret
