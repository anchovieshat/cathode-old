bits 64

global memcpy
global wstrlen
global testout

section .text

wstrlen:
	pushf
	cld
	push rbx
	mov rcx, -1

	xor rax, rax
	repne scasw
	mov rax, -2
	sub rax, rcx

	mov rcx, 2
	mul rcx

	pop rbx
	popf
	ret

memcpy:
	pushf
	cld

	mov rcx, rdx
	rep movsb

	popf
	ret

testout:
	mov dx, 0x3F8
	mov ax, 'a'
	out dx, ax
	ret
