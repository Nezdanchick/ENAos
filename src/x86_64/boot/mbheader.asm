section .multiboot_header

MAGIC equ 0xe85250d6
ARCHITECTURE equ 0
LENGTH equ (mb_header_end - mb_header_start)
CHECKSUM equ 0x100000000 - (MAGIC + ARCHITECTURE + LENGTH)

mb_header_start:
    dd MAGIC
    dd ARCHITECTURE
    dd LENGTH
    ; checksum
    dd CHECKSUM

    ; dw 5    ; framebuffer
    ; dw 0    ; optional tag
    ; dd 20   ; length of tag
    ; dd 640  ; width
    ; dd 480  ; height
    ; dd 32   ; bit depth

    ; dw 0    ; align

    ; end tag
    dw 0
    dw 0
    dd 8
mb_header_end:
