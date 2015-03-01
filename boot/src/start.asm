bits 64

section .text
align 4

global _start
extern _reloc
extern start
extern ImageBase
extern _DYNAMIC

_start:
	sub rsp, 8
	push rcx
	push rdx

	lea rdi, [ImageBase wrt rip]
	lea rsi, [_DYNAMIC wrt rip]

	pop rcx
	pop rdx
	push rcx
	push rdx
	call _reloc

	pop rdi
	pop rsi

	call start
	add rsp, 8

	ret

section .reloc
dd 0
dd 10
dw 0
