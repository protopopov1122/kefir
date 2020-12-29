SECTION .text

%macro declare_opcode 1
global __kefirrt_%1_impl
%endmacro

%macro define_opcode 1
__kefirrt_%1_impl:
%endmacro

%macro define_opcode_stub 1
define_opcode %1
    end_opcode
%endmacro

%define PROGRAM_REG rbx
%define DATA_REG r11
%define DATA2_REG r12
%define STACK_BASE_REG r13
%define INSTR_ARG_PTR PROGRAM_REG + 8

%macro end_opcode 0
    add PROGRAM_REG, 16
    jmp [PROGRAM_REG]
%endmacro

; Opcode definitions
declare_opcode nop
declare_opcode jmp
declare_opcode branch
declare_opcode push
declare_opcode pop
declare_opcode pick
declare_opcode put
declare_opcode drop
declare_opcode iadd
declare_opcode iadd1
declare_opcode isub
declare_opcode imul
declare_opcode idiv
declare_opcode imod
declare_opcode ineg
declare_opcode inot
declare_opcode iand
declare_opcode ior
declare_opcode ixor
declare_opcode ishr
declare_opcode isar
declare_opcode ishl
declare_opcode iequals
declare_opcode igreater
declare_opcode ilesser
declare_opcode iabove
declare_opcode ibelow
declare_opcode band
declare_opcode bor
declare_opcode bnot
declare_opcode trunc1
declare_opcode extend8
declare_opcode extend16
declare_opcode extend32
; Runtime
global __kefirrt_preserve_state
global __kefirrt_restore_state

define_opcode nop
    end_opcode

define_opcode jmp
    mov PROGRAM_REG, [INSTR_ARG_PTR]
    jmp [PROGRAM_REG]

define_opcode branch
    pop DATA_REG
    cmp DATA_REG, 0
    jne __kefirrt_jmp_impl
    end_opcode

define_opcode push
    mov DATA_REG, [INSTR_ARG_PTR]
    push DATA_REG
    end_opcode

define_opcode pop
    add rsp, 8
    end_opcode

define_opcode pick
    mov DATA2_REG, [INSTR_ARG_PTR]
    shl DATA2_REG, 3
    add DATA2_REG, rsp
    mov DATA_REG, [DATA2_REG]
    push DATA_REG
    end_opcode

define_opcode put
    mov DATA2_REG, [INSTR_ARG_PTR]
    shl DATA2_REG, 3
    add DATA2_REG, rsp
    pop DATA_REG
    mov [DATA2_REG], DATA_REG
    end_opcode

define_opcode_stub drop

define_opcode iadd
    pop DATA2_REG
    pop DATA_REG
    add DATA_REG, DATA2_REG
    push DATA_REG
    end_opcode

define_opcode iadd1
    pop DATA_REG
    mov DATA2_REG, [INSTR_ARG_PTR]
    add DATA_REG, DATA2_REG
    push DATA_REG
    end_opcode

define_opcode isub
    pop DATA2_REG
    pop DATA_REG
    sub DATA_REG, DATA2_REG
    push DATA_REG
    end_opcode

define_opcode imul
    pop DATA2_REG
    pop DATA_REG
    imul DATA_REG, DATA2_REG
    push DATA_REG
    end_opcode

define_opcode idiv
    pop DATA_REG
    pop rax
    cqo
    idiv DATA_REG
    push rax
    end_opcode

define_opcode imod
    pop DATA_REG
    pop rax
    cqo
    idiv DATA_REG
    push rdx
    end_opcode

define_opcode ineg
    pop DATA_REG
    neg DATA_REG
    push DATA_REG
    end_opcode

define_opcode inot
    pop DATA_REG
    not DATA_REG
    push DATA_REG
    end_opcode

define_opcode iand
    pop DATA2_REG
    pop DATA_REG
    and DATA_REG, DATA2_REG
    push DATA_REG
    end_opcode

define_opcode ior
    pop DATA2_REG
    pop DATA_REG
    or DATA_REG, DATA2_REG
    push DATA_REG
    end_opcode

define_opcode ixor
    pop DATA2_REG
    pop DATA_REG
    or DATA_REG, DATA2_REG
    push DATA_REG
    end_opcode

define_opcode ishr
    pop DATA2_REG
    pop DATA_REG
    shrx DATA_REG, DATA_REG, DATA2_REG
    push DATA_REG
    end_opcode

define_opcode isar
    pop DATA2_REG
    pop DATA_REG
    sarx DATA_REG, DATA_REG, DATA2_REG
    push DATA_REG
    end_opcode

define_opcode ishl
    pop DATA2_REG
    pop DATA_REG
    shlx DATA_REG, DATA_REG, DATA2_REG
    push DATA_REG
    end_opcode

define_opcode trunc1
    pop DATA_REG
    mov rax, 1
    xor DATA2_REG, DATA2_REG
    cmp DATA_REG, 0
    cmovnz DATA2_REG, rax
    push DATA2_REG
    end_opcode

define_opcode extend8
    pop rax
    movsx rax, al
    push rax
    end_opcode

define_opcode extend16
    pop rax
    movsx rax, ax
    push rax
    end_opcode

define_opcode extend32
    pop rax
    movsx rax, eax
    push rax
    end_opcode

define_opcode iequals
    pop DATA2_REG
    pop DATA_REG
    xor rax, rax
    mov rcx, 1
    cmp DATA_REG, DATA2_REG
    cmove rax, rcx
    push rax
    end_opcode

define_opcode igreater
    pop DATA2_REG
    pop DATA_REG
    xor rax, rax
    mov rcx, 1
    cmp DATA_REG, DATA2_REG
    cmovg rax, rcx
    push rax
    end_opcode

define_opcode ilesser
    pop DATA2_REG
    pop DATA_REG
    xor rax, rax
    mov rcx, 1
    cmp DATA_REG, DATA2_REG
    cmovl rax, rcx
    push rax
    end_opcode

define_opcode iabove
    pop DATA2_REG
    pop DATA_REG
    xor rax, rax
    mov rcx, 1
    cmp DATA_REG, DATA2_REG
    cmova rax, rcx
    push rax
    end_opcode

define_opcode ibelow
    pop DATA2_REG
    pop DATA_REG
    xor rax, rax
    mov rcx, 1
    cmp DATA_REG, DATA2_REG
    cmovb rax, rcx
    push rax
    end_opcode

define_opcode band
    pop DATA2_REG
    pop DATA_REG
    xor rax, rax
    mov rcx, 1
    and DATA_REG, DATA2_REG
    cmovnz rax, rcx
    push rax
    end_opcode

define_opcode bor
    pop DATA2_REG
    pop DATA_REG
    xor rax, rax
    mov rcx, 1
    or DATA_REG, DATA2_REG
    cmovnz rax, rcx
    push rax
    end_opcode

define_opcode bnot
    pop DATA_REG
    xor DATA2_REG, DATA2_REG
    mov rax, 1
    test DATA_REG, DATA_REG
    cmovz DATA2_REG, rax
    push DATA2_REG
    end_opcode

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