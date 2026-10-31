global gdt64
global gdt64.pointer
global gdt64.code_segment
global gdt64.data_segment

section .boot.data
align 16
gdt64:
	dq 0	; entry
.code_segment equ $ - gdt64
	dq (1 << 41) | (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
.data_segment equ $ - gdt64
	dq (1 << 41) | (1 << 44) | (1 << 47)
.pointer:
	dw $ - gdt64 - 1
	dq gdt64
