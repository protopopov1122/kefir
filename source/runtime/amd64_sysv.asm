SECTION .text

%macro next_instr 0
    add rbx, 16
    jmp [rbx]
%endmacro

; Opcode definitions
global __kefirrt_nop_impl
global __kefirrt_push_impl
global __kefirrt_pop_impl
global __kefirrt_iadd_impl
global __kefirrt_ret_impl
; Runtime
global __kefirrt_preserve_state
global __kefirrt_restore_state

__kefirrt_nop_impl:
    next_instr

__kefirrt_push_impl:
    mov rax, [rbx + 8]
    push rax
    next_instr

__kefirrt_pop_impl:
    add rsp, 8
    next_instr

__kefirrt_iadd_impl:
    pop rcx
    pop rax
    add rax, rcx
    push rax
    next_instr

__kefirrt_ret_impl:
    pop rax
    next_instr

; Runtime helpers
__kefirrt_preserve_state:
    pop r11
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15
    sub rsp, 8
    fstcw [rsp]
    jmp r11

__kefirrt_restore_state:
    fldcw [rsp]
    add rsp, 8
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret