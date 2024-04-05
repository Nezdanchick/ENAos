section .multiboot_header

MAGIC equ 0xe85250d6
ARCH equ 0
LENGTH equ (mb_header_end - mb_header_start)

mb_header_start:
    dd MAGIC
    dd ARCH
    dd LENGTH
    ; checksum
    dd 0x100000000 - (MAGIC + ARCH + LENGTH)

    ; dw 5    ; framebuffer
    ; dw 0    ; optional tag
    ; dd 20   ; length of tag
    ; dd 320 ; width
    ; dd 200  ; height
    ; dd 32   ; bit depth

    ; dw 0 ; align

    ; end tag
    dw 0
    dw 0
    dd 8
mb_header_end:
