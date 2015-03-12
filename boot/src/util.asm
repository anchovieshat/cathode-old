bits 64

global memcpy
global memset
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

memset:
	pushf
	cld

	mov rax, rsi
	mov rcx, rdx
	rep stosb

	mov rax, rdi

	popf
	ret
