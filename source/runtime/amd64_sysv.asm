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
%define TMP_REG r11
%define DATA_REG r12
%define DATA2_REG r13
%define STACK_BASE_REG r14
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
declare_opcode insert
declare_opcode xchg
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
declare_opcode offsetptr
declare_opcode elementptr
declare_opcode load8u
declare_opcode load8i
declare_opcode load16u
declare_opcode load16i
declare_opcode load32u
declare_opcode load32i
declare_opcode load64
declare_opcode store8
declare_opcode store16
declare_opcode store32
declare_opcode store64
declare_opcode bzero
declare_opcode bcopy
declare_opcode getlocals
declare_opcode f32add
declare_opcode f32sub
declare_opcode f32mul
declare_opcode f32div
declare_opcode f32neg
declare_opcode f64add
declare_opcode f64sub
declare_opcode f64mul
declare_opcode f64div
declare_opcode f64neg
declare_opcode f32equals
declare_opcode f32greater
declare_opcode f32lesser
declare_opcode f64equals
declare_opcode f64greater
declare_opcode f64lesser
declare_opcode f32cint
declare_opcode f64cint
declare_opcode intcf32
declare_opcode intcf64
declare_opcode uintcf32
declare_opcode uintcf64
declare_opcode f32cf64
declare_opcode f64cf32
; Runtime
global __kefirrt_preserve_state
global __kefirrt_restore_state
global __kefirrt_save_registers
global __kefirrt_load_integer_vararg
global __kefirrt_load_sse_vararg
global __kefirrt_copy_vararg

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

define_opcode insert
    mov rdx, [rsp]
    mov rcx, [INSTR_ARG_PTR]
    mov rax, rcx
    inc rcx
    shl rax, 3
    add rax, rsp
    mov rsi, rsp
    mov rdi, rsp
    sub rdi, 8 
    cld
    rep movsq
    mov [rax], rdx
    end_opcode

define_opcode xchg
    mov DATA2_REG, [INSTR_ARG_PTR]
    shl DATA2_REG, 3
    add DATA2_REG, rsp
    mov DATA_REG, [DATA2_REG]
    xchg DATA_REG, [rsp]
    mov [DATA2_REG], DATA_REG
    end_opcode

define_opcode drop
    mov rcx, [INSTR_ARG_PTR]
    mov rdi, rcx
    shl rdi, 3
    add rdi, rsp
    mov rsi, rdi
    sub rsi, 8
    std
    rep movsq
    cld
    add rsp, 8
    end_opcode

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
    xor DATA_REG, DATA2_REG
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

define_opcode offsetptr
    pop DATA_REG
    add DATA_REG, [INSTR_ARG_PTR]
    push DATA_REG
    end_opcode

define_opcode elementptr
    pop DATA2_REG
    pop DATA_REG
    imul DATA2_REG, [INSTR_ARG_PTR]
    add DATA_REG, DATA2_REG
    push DATA_REG
    end_opcode

define_opcode load8u
    pop DATA2_REG
    movzx DATA_REG, byte [DATA2_REG]
    push DATA_REG
    end_opcode

define_opcode load8i
    pop DATA2_REG
    movsx DATA_REG, byte [DATA2_REG]
    push DATA_REG
    end_opcode

define_opcode load16u
    pop DATA2_REG
    movzx DATA_REG, word [DATA2_REG]
    push DATA_REG
    end_opcode

define_opcode load16i
    pop DATA2_REG
    movsx DATA_REG, word [DATA2_REG]
    push DATA_REG
    end_opcode

define_opcode load32u
    pop DATA2_REG
    mov eax, [DATA2_REG]
    push rax
    end_opcode

define_opcode load32i
    pop DATA2_REG
    mov eax, [DATA2_REG]
    movsx DATA_REG, eax
    push DATA_REG
    end_opcode

define_opcode load64
    pop DATA2_REG
    mov DATA_REG, [DATA2_REG]
    push DATA_REG
    end_opcode

define_opcode store8
    pop rax
    pop DATA2_REG
    mov [DATA2_REG], al
    end_opcode

define_opcode store16
    pop rax
    pop DATA2_REG
    mov [DATA2_REG], ax
    end_opcode

define_opcode store32
    pop rax
    pop DATA2_REG
    mov [DATA2_REG], eax
    end_opcode

define_opcode store64
    pop rax
    pop DATA2_REG
    mov [DATA2_REG], rax
    end_opcode

define_opcode bzero
    mov rcx, [INSTR_ARG_PTR]
    pop rdi
__kefirrt_bzero_loop_begin:
    cmp rcx, 0
    je __kefirrt_bzero_loop_end
    mov byte [rdi], 0
    inc rdi
    dec rcx
    jmp __kefirrt_bzero_loop_begin
__kefirrt_bzero_loop_end:
    end_opcode

define_opcode bcopy
    mov rcx, [INSTR_ARG_PTR]
    pop rsi
    pop rdi
    rep movsb
    end_opcode

define_opcode getlocals
    mov DATA_REG, [INSTR_ARG_PTR]
    add DATA_REG, STACK_BASE_REG
    push DATA_REG
    end_opcode

define_opcode f32add
    movss xmm0, [rsp + 8]
    movss xmm1, [rsp]
    addss xmm0, xmm1
    movss [rsp + 8], xmm0
    add rsp, 8
    end_opcode

define_opcode f32sub
    movss xmm0, [rsp + 8]
    movss xmm1, [rsp]
    subss xmm0, xmm1
    movss [rsp + 8], xmm0
    add rsp, 8
    end_opcode

define_opcode f32mul
    movss xmm0, [rsp + 8]
    movss xmm1, [rsp]
    mulss xmm0, xmm1
    movss [rsp + 8], xmm0
    add rsp, 8
    end_opcode

define_opcode f32div
    movss xmm0, [rsp + 8]
    movss xmm1, [rsp]
    divss xmm0, xmm1
    movss [rsp + 8], xmm0
    add rsp, 8
    end_opcode

define_opcode f32neg
    movss xmm0, [rsp]
    xorps xmm0, oword [__kefirrt_f32neg_constant]
    movss [rsp], xmm0
    end_opcode
__kefirrt_f32neg_constant:
    dd 0x80000000
    dd 0x80000000
    dd 0x80000000
    dd 0x80000000

define_opcode f64add
    movsd xmm0, [rsp]
    movsd xmm1, [rsp + 8]
    addsd xmm0, xmm1
    movsd [rsp + 8], xmm0
    add rsp, 8
    end_opcode

define_opcode f64sub
    movsd xmm0, [rsp + 8]
    movsd xmm1, [rsp]
    subsd xmm0, xmm1
    movsd [rsp + 8], xmm0
    add rsp, 8
    end_opcode

define_opcode f64mul
    movsd xmm0, [rsp]
    movsd xmm1, [rsp + 8]
    mulsd xmm0, xmm1
    movsd [rsp + 8], xmm0
    add rsp, 8
    end_opcode

define_opcode f64div
    movsd xmm0, [rsp + 8]
    movsd xmm1, [rsp]
    divsd xmm0, xmm1
    movsd [rsp + 8], xmm0
    add rsp, 8
    end_opcode

define_opcode f64neg
    movsd xmm0, [rsp]
    xorps xmm0, oword [__kefirrt_f64neg_constant]
    movsd [rsp], xmm0
    end_opcode
__kefirrt_f64neg_constant:
    dq 0x8000000000000000
    dq 0x8000000000000000

define_opcode f32equals
    movss xmm0, [rsp + 8]
    movss xmm1, [rsp]
    cmpeqss xmm0, xmm1
    movd eax, xmm0
    and eax, 1
    push rax
    end_opcode

define_opcode f32greater
    movss xmm0, [rsp + 8]
    movss xmm1, [rsp]
    comiss xmm0, xmm1
    seta al
    and eax, 1
    push rax
    end_opcode

define_opcode f32lesser
    movss xmm0, [rsp + 8]
    movss xmm1, [rsp]
    cmpltss xmm0, xmm1
    movd eax, xmm0
    and eax, 1
    push rax
    end_opcode

define_opcode f64equals
    movsd xmm0, [rsp + 8]
    movsd xmm1, [rsp]
    cmpeqsd xmm0, xmm1
    movd eax, xmm0
    and eax, 1
    push rax
    end_opcode

define_opcode f64greater
    movsd xmm0, [rsp + 8]
    movsd xmm1, [rsp]
    comisd xmm0, xmm1
    seta al
    and eax, 1
    push rax
    end_opcode

define_opcode f64lesser
    movsd xmm0, [rsp + 8]
    movsd xmm1, [rsp]
    cmpltsd xmm0, xmm1
    movd eax, xmm0
    and eax, 1
    push rax
    end_opcode

define_opcode f32cint
    movss xmm0, [rsp]
    cvttss2si rax, xmm0
    mov [rsp], rax
    end_opcode

define_opcode f64cint
    movsd xmm0, [rsp]
    cvttsd2si rax, xmm0
    mov [rsp], rax
    end_opcode

define_opcode intcf32
    mov rax, [rsp]
    pxor xmm0, xmm0
    cvtsi2ss xmm0, rax
    movss [rsp], xmm0
    end_opcode

define_opcode intcf64
    mov rax, [rsp]
    pxor xmm0, xmm0
    cvtsi2sd xmm0, rax
    movsd [rsp], xmm0
    end_opcode

define_opcode uintcf32
    mov DATA_REG, [rsp]
    pxor xmm0, xmm0
    test DATA_REG, DATA_REG
    js __kefirrt_uintcf32_signed
    cvtsi2ss        xmm0, DATA_REG
    movss [rsp], xmm0
    end_opcode
__kefirrt_uintcf32_signed:
    mov DATA2_REG, DATA_REG
    shr DATA2_REG, 1
    and DATA_REG, 1
    or  DATA_REG, DATA2_REG
    cvtsi2ss xmm0, DATA_REG
    addss xmm0, xmm0
    movss [rsp], xmm0
    end_opcode

define_opcode uintcf64
    mov DATA_REG, [rsp]
    pxor xmm0, xmm0
    test DATA_REG, DATA_REG
    js __kefirrt_uintcf64_signed
    cvtsi2sd xmm0, DATA_REG
    movsd [rsp], xmm0
    end_opcode
__kefirrt_uintcf64_signed:
    mov DATA2_REG, DATA_REG
    shr DATA2_REG, 1
    and DATA_REG, 1
    or  DATA_REG, DATA2_REG
    cvtsi2sd xmm0, DATA_REG
    addsd xmm0, xmm0
    movsd [rsp], xmm0
    end_opcode

define_opcode f32cf64
    movss xmm0, [rsp]
    cvtss2sd xmm0, xmm0
    movsd [rsp], xmm0
    end_opcode

define_opcode f64cf32
    movsd xmm0, [rsp]
    cvtsd2ss xmm0, xmm0
    movss [rsp], xmm0
    end_opcode

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
    sub rsp, 24
    fstcw [rsp + 16]
    stmxcsr [rsp + 8]
    jmp r11

__kefirrt_restore_state:
    ldmxcsr [rsp + 8]
    fldcw [rsp + 16]
    add rsp, 24
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

__kefirrt_save_registers:
    test    al, al
    je      __kefirrt_save_int_registers
    movaps  oword [r12 + 48], xmm0
    movaps  oword [r12 + 64], xmm1
    movaps  oword [r12 + 80], xmm2
    movaps  oword [r12 + 96], xmm3
    movaps  oword [r12 + 112], xmm4
    movaps  oword [r12 + 128], xmm5
    movaps  oword [r12 + 144], xmm6
    movaps  oword [r12 + 160], xmm7
__kefirrt_save_int_registers:
    mov     qword [r12], rdi
    mov     qword [r12 + 8], rsi
    mov     qword [r12 + 16], rdx
    mov     qword [r12 + 24], rcx
    mov     qword [r12 + 32], r8
    mov     qword [r12 + 40], r9
    ret

__kefirrt_load_integer_vararg:
    mov eax, dword [DATA_REG]
    cmp eax, 48
    jae __kefirrt_load_integer_vararg_stack
    lea edx, [eax + 8]
    add rax, [DATA_REG + 2*8]
    mov [DATA_REG], edx
    jmp __kefirrt_load_integer_vararg_fetch
__kefirrt_load_integer_vararg_stack:
    mov rax, [DATA_REG + 8]
    lea rdx, [rax + 8]
    mov [DATA_REG + 8], rdx
__kefirrt_load_integer_vararg_fetch:
    mov DATA2_REG, [rax]
    ret

__kefirrt_load_sse_vararg:
    mov eax, dword [DATA_REG + 4]
    cmp eax, 176
    jae __kefirrt_load_sse_vararg_stack
    lea edx, [eax + 16]
    add rax, [DATA_REG + 2*8]
    mov [DATA_REG + 4], edx
    movaps xmm0, oword [rax]
    ret
__kefirrt_load_sse_vararg_stack:
    mov rax, [DATA_REG + 8]
    lea rdx, [rax + 8]
    mov [DATA_REG + 8], rdx
    movlps xmm0, qword [rax]
    ret

__kefirrt_copy_vararg:
    pop rsi
    pop rdi
    movsq
    movsq
    movsq
    end_opcode