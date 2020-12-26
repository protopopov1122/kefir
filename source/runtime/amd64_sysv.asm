SECTION .text

%define PROGRAM_REG rbx
%define DATA_REG r11
%define DATA2_REG r12
%define STACK_BASE_REG r12

%macro next_instr 0
    add PROGRAM_REG, 16
    jmp [PROGRAM_REG]
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
    mov DATA_REG, [rbx + 8]
    push DATA_REG
    next_instr

__kefirrt_pop_impl:
    add rsp, 8
    next_instr

__kefirrt_iadd_impl:
    pop DATA2_REG
    pop DATA_REG
    add DATA_REG, DATA2_REG
    push DATA_REG
    next_instr

__kefirrt_ret_impl:
    pop rax
    next_instr

; Runtime helpers
__kefirrt_preserve_state:
    pop DATA_REG
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15
    sub rsp, 8
    fstcw [rsp]
    jmp DATA_REG

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