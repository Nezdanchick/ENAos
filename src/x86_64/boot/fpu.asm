global enable_fpu

segment .text
bits 32
enable_fpu:
    ; init FPU
    fninit
    fldcw [fcw]

    ; enable SSE
    mov eax, cr0
    and al, ~0x04
    or al, 0x22
    mov cr0, eax
    mov eax, cr4
    or ax, 0x600
    mov cr4, eax
    ret

segment .data
fcw: dw 0x037F
