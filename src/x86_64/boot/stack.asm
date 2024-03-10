global stack_top
global stack_bottom

section .bss
stack_bottom:
	resb 16 * 1024
stack_top: