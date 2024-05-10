global start64

extern stack_top
extern init
extern main

section .text
bits 64

start64:
    mov ax, 0x10    ; data segment
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; multiboot values are 32 bit size
    mov edi, [stack_top - 4]    ; multiboot2 address
    mov esi, [stack_top - 8]    ; multiboot2 magic
    add rsp, 8                  ; clear stack

	call init   ; (dword addr, dword magic)
	call main

.halt:
    hlt
    jmp .halt
