global start

; stack.asm
extern stack_top
; longmode.asm
extern check_cpuid
extern check_longmode
; gdt64.asm
extern gdt64.pointer
extern gdt64.code_segment
; paging.asm
extern setup_page_tables
extern enable_paging

extern start64	; 64 bit entry point

section .text
bits 32

start:
	mov esp, stack_top				; setup stack
	push ebx	; multiboot 
	push eax

	call check_multiboot
	call check_cpuid
	call check_longmode

	call setup_page_tables
	call enable_paging

	lgdt [gdt64.pointer]			; setup gdt64
	jmp gdt64.code_segment:start64	; long jump

	hlt

check_multiboot:
	cmp eax, 0x36d76289
	jne .no_multiboot
	ret
.no_multiboot:	; show error
	mov dword [0xb8000], 0x4f424f4d	; red "MB"
	hlt
