global stack_top
global stack_bottom

section .bss
align 16
stack_bottom
	resb 1024*2
stack_top