global start64
extern main

section .text
bits 64

start64:
    xor ax, ax
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

	call main

	hlt