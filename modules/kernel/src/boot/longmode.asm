global check_cpuid
global check_longmode

section .text
bits 32

check_cpuid:
	pushfd				; flags => stack
	pop eax
	mov ecx, eax
	xor eax, 1 << 21	; 21st bit is cpuid
	push eax
	popfd				; flags <= stack
	pushfd
	pop eax
	push ecx
	popfd
	cmp eax, ecx
	je .no_cpuid
	ret
.no_cpuid:	; show error
	mov dword [0xb8000], 0x4f444f49	; red "ID"
	hlt

check_longmode:
	mov eax, 0x80000000
	cpuid
	cmp eax, 0x80000001
	jb .no_longmode	; is greater

	mov eax, 0x80000001
	cpuid
	test edx, 1 << 29
	jz .no_longmode
	ret
.no_longmode:	; show error
	mov dword [0xb8000], 0x4f4d4f4c	; red "LM"
	hlt
