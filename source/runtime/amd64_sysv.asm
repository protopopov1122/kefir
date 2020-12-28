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
%define STACK_BASE_REG r12

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
declare_opcode ishl
declare_opcode cmp
declare_opcode band
declare_opcode bor
declare_opcode bnot
declare_opcode asbool
declare_opcode trunc
; Runtime
global __kefirrt_preserve_state
global __kefirrt_restore_state

define_opcode nop
    end_opcode

define_opcode jmp
    mov PROGRAM_REG, [PROGRAM_REG + 8]
    jmp [PROGRAM_REG]

define_opcode branch
    pop DATA_REG
    cmp DATA_REG, 0
    jne __kefirrt_jmp_impl
    end_opcode

define_opcode push
    mov DATA_REG, [PROGRAM_REG + 8]
    push DATA_REG
    end_opcode

define_opcode pop
    add rsp, 8
    end_opcode

define_opcode_stub pick
define_opcode_stub drop

define_opcode iadd
    pop DATA2_REG
    pop DATA_REG
    add DATA_REG, DATA2_REG
    push DATA_REG
    end_opcode

define_opcode iadd1
    pop DATA_REG
    mov DATA2_REG, [PROGRAM_REG + 8]
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

define_opcode_stub inot
define_opcode_stub iand
define_opcode_stub ior
define_opcode_stub ixor
define_opcode_stub ishr
define_opcode_stub ishl
define_opcode_stub cmp
define_opcode_stub band
define_opcode_stub bor
define_opcode_stub bnot
define_opcode_stub asbool
define_opcode_stub trunc

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