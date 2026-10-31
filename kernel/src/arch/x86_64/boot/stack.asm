global stack_top
global stack_bottom

section .boot.bss nobits alloc write
align 16
stack_bottom:
	resb 1024*32
stack_top:
