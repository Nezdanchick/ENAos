; optimized code from https://github.com/And1sS/sos/

section .text
bits 64

; C handlers defined in isrs.c
extern handle_interrupt

%macro save_ds 0
    mov bx, ds
    push rbx
    mov rbx, 0x10
    mov ds, bx
%endmacro

%macro restore_ds 0
    pop rbx
    mov ds, bx
%endmacro

%macro save_state 0
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rbp
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax
    save_ds
%endmacro

%macro restore_state 0
    restore_ds
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15

    add rsp, 8 ; remove error code from stack
%endmacro

common_irq_stub:
    mov rsi, [rsp + 16 * 8] ; stack top initially holds error code,
                            ; we move it here so that we don't scratch
                            ; rsi value before saving state

    mov rdx, rsp ; cpu_context pointer
    call handle_interrupt

    restore_state

    iretq

; Exception
%macro esr 1
isr_%1:
    save_state

    mov rdi, %1  ; interrupt number
    jmp common_irq_stub
%endmacro

; Interrupt
%macro isr 1
isr_%1:
    push 0 ; push fake error code
    save_state

    mov rdi, %1  ; interrupt number
    jmp common_irq_stub
%endmacro

isr 0
isr 1
isr 2
isr 3
isr 4
isr 5
isr 6
isr 7
esr 8
isr 9
esr 10
esr 11
esr 12
esr 13
esr 14
isr 15
isr 16
esr 17
isr 18
isr 19
isr 20
esr 21
isr 22
isr 23
isr 24
isr 25
isr 26
isr 27
isr 28
esr 29
esr 30
isr 31

%assign i 32
%rep    256 - 32
    isr i
%assign i i+1
%endrep

global isr_table
isr_table:
%assign i 0
%rep    256
    dq isr_%+i
%assign i i+1
%endrep
