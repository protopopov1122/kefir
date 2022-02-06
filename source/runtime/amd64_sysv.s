# SPDX-License-Identifier: BSD-3-Clause
#
# Copyright 2020-2021 Jevgenijs Protopopovs
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

.intel_syntax noprefix
.section .text

.macro declare_runtime identifier
.global \identifier
.hidden \identifier
.endm

.macro declare_opcode identifier
declare_runtime __kefirrt_\identifier\()_impl
.endm

.macro define_opcode identifier
__kefirrt_\identifier\()_impl:
.endm

.macro define_opcode_stub identifier
define_opcode \identifier
    end_opcode
.endm

.equ PROGRAM_REG, rbx
.equ TMP_REG, r11
.equ DATA_REG, r12
.equ DATA2_REG, r13
.equ STACK_BASE_REG, r14
.equ INSTR_ARG_PTR, PROGRAM_REG + 8
.equ INSTR_ARG_PTR4, PROGRAM_REG + 12

.macro end_opcode
    add PROGRAM_REG, 16
    jmp [PROGRAM_REG]
.endm

# Opcode definitions
declare_opcode nop
declare_opcode jmp
declare_opcode ijmp
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
declare_opcode udiv
declare_opcode umod
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
declare_opcode load24u
declare_opcode load32u
declare_opcode load32i
declare_opcode load40u
declare_opcode load48u
declare_opcode load56u
declare_opcode load64
declare_opcode loadld
declare_opcode store8
declare_opcode store16
declare_opcode store24
declare_opcode store32
declare_opcode store40
declare_opcode store48
declare_opcode store56
declare_opcode store64
declare_opcode storeld
declare_opcode bzero
declare_opcode bcopy
declare_opcode extubits
declare_opcode extsbits
declare_opcode insertbits
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
declare_opcode ldadd
declare_opcode ldsub
declare_opcode ldmul
declare_opcode lddiv
declare_opcode ldneg
declare_opcode f32equals
declare_opcode f32greater
declare_opcode f32lesser
declare_opcode f64equals
declare_opcode f64greater
declare_opcode f64lesser
declare_opcode ldequals
declare_opcode ldgreater
declare_opcode ldlesser
declare_opcode ldtrunc1
declare_opcode f32cint
declare_opcode f64cint
declare_opcode intcf32
declare_opcode intcf64
declare_opcode uintcf32
declare_opcode uintcf64
declare_opcode f32cf64
declare_opcode f64cf32
declare_opcode ldcint
declare_opcode intcld
declare_opcode uintcld
declare_opcode f32cld
declare_opcode f64cld
declare_opcode ldcf32
declare_opcode ldcf64
declare_opcode alloca
declare_opcode pushscope
declare_opcode popscope
# Runtime
declare_runtime __kefirrt_preserve_state
declare_runtime __kefirrt_generic_prologue
declare_runtime __kefirrt_restore_state
declare_runtime __kefirrt_save_registers
declare_runtime __kefirrt_load_integer_vararg
declare_runtime __kefirrt_load_sse_vararg
declare_runtime __kefirrt_copy_vararg
declare_runtime __kefirrt_load_long_double_vararg

define_opcode nop
    end_opcode

define_opcode jmp
    mov PROGRAM_REG, [INSTR_ARG_PTR]
    jmp [PROGRAM_REG]

define_opcode ijmp
    pop PROGRAM_REG
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

define_opcode udiv
    pop DATA_REG
    pop rax
    xor rdx, rdx
    div DATA_REG
    push rax
    end_opcode

define_opcode umod
    pop DATA_REG
    pop rax
    xor rdx, rdx
    div DATA_REG
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
    movzx DATA_REG, byte ptr [DATA2_REG]
    push DATA_REG
    end_opcode

define_opcode load8i
    pop DATA2_REG
    movsx DATA_REG, byte ptr [DATA2_REG]
    push DATA_REG
    end_opcode

define_opcode load16u
    pop DATA2_REG
    movzx DATA_REG, word ptr [DATA2_REG]
    push DATA_REG
    end_opcode

define_opcode load16i
    pop DATA2_REG
    movsx DATA_REG, word ptr [DATA2_REG]
    push DATA_REG
    end_opcode

define_opcode load24u
    pop TMP_REG
    movzx DATA_REG, word ptr [TMP_REG]
    movzx DATA2_REG, byte ptr [TMP_REG + 2]
    shl DATA2_REG, 16
    or DATA_REG, DATA2_REG
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

define_opcode load40u
    pop TMP_REG
    mov eax, [TMP_REG]
    movzx DATA2_REG, byte ptr [TMP_REG + 4]
    shl DATA2_REG, 32
    or rax, DATA2_REG
    push rax
    end_opcode

define_opcode load48u
    pop TMP_REG
    mov eax, [TMP_REG]
    movzx DATA2_REG, word ptr [TMP_REG + 4]
    shl DATA2_REG, 32
    or rax, DATA2_REG
    push rax
    end_opcode

define_opcode load56u
    pop TMP_REG
    mov eax, [TMP_REG]
    movzx DATA2_REG, word ptr [TMP_REG + 4]
    shl DATA2_REG, 32
    or rax, DATA2_REG
    movzx DATA2_REG, byte ptr [TMP_REG + 6]
    shl DATA2_REG, 48
    or rax, DATA2_REG
    push rax
    end_opcode

define_opcode load64
    pop DATA2_REG
    mov DATA_REG, [DATA2_REG]
    push DATA_REG
    end_opcode

define_opcode loadld
    pop DATA2_REG
    mov DATA_REG, [DATA2_REG + 8]
    push DATA_REG
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

define_opcode store24
    pop rax
    pop DATA2_REG
    mov [DATA2_REG], ax
    shr rax, 16
    mov [DATA2_REG + 2], al
    end_opcode

define_opcode store32
    pop rax
    pop DATA2_REG
    mov [DATA2_REG], eax
    end_opcode

define_opcode store40
    pop rax
    pop DATA2_REG
    mov [DATA2_REG], eax
    shr rax, 32
    mov [DATA2_REG + 4], al
    end_opcode

define_opcode store48
    pop rax
    pop DATA2_REG
    mov [DATA2_REG], eax
    shr rax, 32
    mov [DATA2_REG + 4], ax
    end_opcode

define_opcode store56
    pop rax
    pop DATA2_REG
    mov [DATA2_REG], eax
    shr rax, 32
    mov [DATA2_REG + 4], ax
    shr rax, 16
    mov [DATA2_REG + 6], al
    end_opcode

define_opcode store64
    pop rax
    pop DATA2_REG
    mov [DATA2_REG], rax
    end_opcode

define_opcode storeld
    pop DATA_REG
    pop DATA2_REG
    pop rax
    mov [rax], DATA_REG
    mov [rax + 8], DATA2_REG
    end_opcode

define_opcode bzero
    mov rcx, [INSTR_ARG_PTR]
    pop rdi
__kefirrt_bzero_loop_begin:
    cmp rcx, 0
    je __kefirrt_bzero_loop_end
    mov byte ptr [rdi], 0
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

define_opcode extubits
    mov edx, [INSTR_ARG_PTR]            # Offset
    mov ecx, [INSTR_ARG_PTR4]           # Width
    mov r11, 1                          # Mask = (1 << Width) - 1
    shlx r11, r11, rcx
    sub r11, 1
    pop rax                             # Value = (Bitfield >> Offset) & Mask
    shrx rax, rax, rdx
    and rax, r11
    push rax
    end_opcode

define_opcode extsbits
    mov edx, [INSTR_ARG_PTR]            # Offset
    mov ecx, [INSTR_ARG_PTR4]           # Width
    add rdx, rcx                        # rdx = 64 - (Width + Offset)
    neg rdx
    add rdx, 64
    neg rcx                             # rcx = 64 - Width
    add rcx, 64
    pop rax                             # Value = (Bitfield << rdx)) >> rcx
    shlx rax, rax, rdx
    sarx rax, rax, rcx
    push rax
    end_opcode

define_opcode insertbits
    mov edx, [INSTR_ARG_PTR]            # Offset
    mov ecx, [INSTR_ARG_PTR4]           # Width
    pop rsi                             # Value
    pop rdi                             # Bit-field

    # Mask value: Value = Value << (64 - Width) >> (64 - Width - Offset)
    mov rax, 64                        # RAX = 64 - Width
    sub rax, rcx
    shlx rsi, rsi, rax
    sub rax, rdx                        # RAX = 64 - Width - Offset
    shrx rsi, rsi, rax

    # Mask bit-field: Bit-field = Bit-field & ~(((1 << Width) - 1) << Offset)
    mov rax, 1
    shlx rax, rax, rcx
    sub rax, 1
    shlx rax, rax, rdx
    not rax
    and rdi, rax

    # Bit-field = Bit-field | Value
    or rdi, rsi
    push rdi
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
    xorps xmm0, XMMWORD PTR __kefirrt_f32neg_constant[rip]
    movss [rsp], xmm0
    end_opcode
.align 16
__kefirrt_f32neg_constant:
    .long 0x80000000
    .long 0x80000000
    .long 0x80000000
    .long 0x80000000

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
    xorps xmm0, XMMWORD PTR __kefirrt_f64neg_constant[rip]
    movsd [rsp], xmm0
    end_opcode
.align 16
__kefirrt_f64neg_constant:
    .quad 0x8000000000000000
    .quad 0x8000000000000000

define_opcode ldadd
    fld TBYTE PTR [rsp + 16]
    fld TBYTE PTR [rsp]
    faddp
    fstp TBYTE PTR [rsp + 16]
    add rsp, 16
    end_opcode

define_opcode ldsub
    fld TBYTE PTR [rsp + 16]
    fld TBYTE PTR [rsp]
    fsubp
    fstp TBYTE PTR [rsp + 16]
    add rsp, 16
    end_opcode

define_opcode ldmul
    fld TBYTE PTR [rsp + 16]
    fld TBYTE PTR [rsp]
    fmulp
    fstp TBYTE PTR [rsp + 16]
    add rsp, 16
    end_opcode

define_opcode lddiv
    fld TBYTE PTR [rsp + 16]
    fld TBYTE PTR [rsp]
    fdivp
    fstp TBYTE PTR [rsp + 16]
    add rsp, 16
    end_opcode

define_opcode ldneg
    fld TBYTE PTR [rsp]
    fchs
    fstp TBYTE PTR [rsp]
    end_opcode

define_opcode f32equals
    movss xmm0, [rsp + 8]
    movss xmm1, [rsp]
    cmpeqss xmm0, xmm1
    movd eax, xmm0
    and eax, 1
    add rsp, 8
    mov [rsp], rax
    end_opcode

define_opcode f32greater
    movss xmm0, [rsp + 8]
    movss xmm1, [rsp]
    comiss xmm0, xmm1
    seta al
    and eax, 1
    add rsp, 8
    mov [rsp], rax
    end_opcode

define_opcode f32lesser
    movss xmm0, [rsp + 8]
    movss xmm1, [rsp]
    cmpltss xmm0, xmm1
    movd eax, xmm0
    and eax, 1
    add rsp, 8
    mov [rsp], rax
    end_opcode

define_opcode f64equals
    movsd xmm0, [rsp + 8]
    movsd xmm1, [rsp]
    cmpeqsd xmm0, xmm1
    movd eax, xmm0
    and eax, 1
    add rsp, 8
    mov [rsp], rax
    end_opcode

define_opcode f64greater
    movsd xmm0, [rsp + 8]
    movsd xmm1, [rsp]
    comisd xmm0, xmm1
    seta al
    and eax, 1
    add rsp, 8
    mov [rsp], rax
    end_opcode

define_opcode f64lesser
    movsd xmm0, [rsp + 8]
    movsd xmm1, [rsp]
    cmpltsd xmm0, xmm1
    movd eax, xmm0
    and eax, 1
    add rsp, 8
    mov [rsp], rax
    end_opcode

define_opcode ldequals
    xor rax, rax
    fld TBYTE PTR [rsp + 16]
    fld TBYTE PTR [rsp]
    fucomip st(0), st(1)
    fstp st(0)
    setnp dl
    sete al
    and al, dl
    mov [rsp + 24], rax
    add rsp, 24
    end_opcode

define_opcode ldgreater
    xor rax, rax
    fld TBYTE PTR [rsp]
    fld TBYTE PTR [rsp + 16]
    fucomip st(0), st(1)
    fstp st(0)
    seta al
    mov [rsp + 24], rax
    add rsp, 24
    end_opcode

define_opcode ldlesser
    xor rax, rax
    fld TBYTE PTR [rsp + 16]
    fld TBYTE PTR [rsp]
    fucomip st(0), st(1)
    fstp st(0)
    seta al
    mov [rsp + 24], rax
    add rsp, 24
    end_opcode

define_opcode ldtrunc1
    xor rax, rax
    fldz
    fld TBYTE PTR [rsp]
    fucomip st(0), st(1)
    fstp st(0)
    setnp dl
    sete al
    and al, dl
    xor rax, 1
    mov [rsp + 8], rax
    add rsp, 8
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
    cvtsi2ss xmm0, DATA_REG
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

define_opcode ldcint    
    fld TBYTE PTR [rsp]
    fnstcw WORD PTR [rsp]
    movzx eax, WORD PTR [rsp]
    or eax, 3072
    mov WORD PTR [rsp + 8], ax
    fldcw WORD PTR [rsp + 8]
    fistp QWORD PTR [rsp + 8]
    fldcw WORD PTR [rsp]
    add rsp, 8
    end_opcode

define_opcode intcld
    fild QWORD PTR [rsp]
    sub rsp, 8
    fstp TBYTE PTR [rsp]
    end_opcode

define_opcode uintcld
    fild QWORD PTR [rsp]
    mov rax, [rsp]
    sub rsp, 8
    test rax, rax
    js __kefirrt_uintcld_signed
    fstp TBYTE PTR [rsp]
    end_opcode
__kefirrt_uintcld_signed:
    fadd DWORD PTR [__kefirrt_uintcld_constant]
    fstp TBYTE PTR [rsp]
    end_opcode
__kefirrt_uintcld_constant:
    .long   1602224128

define_opcode f32cld
    fld DWORD PTR [rsp]
    sub rsp, 8
    fstp TBYTE PTR [rsp]
    end_opcode

define_opcode f64cld
    fld QWORD PTR [rsp]
    sub rsp, 8
    fstp TBYTE PTR [rsp]
    end_opcode

define_opcode ldcf32
    fld TBYTE PTR [rsp]
    add rsp, 8
    fstp DWORD PTR [rsp]
    xor eax, eax
    mov DWORD PTR [rsp + 4], eax
    end_opcode

define_opcode ldcf64
    fld TBYTE PTR [rsp]
    add rsp, 8
    fstp QWORD PTR [rsp]
    end_opcode

define_opcode alloca
    mov r12, [INSTR_ARG_PTR]
    cmp r12, 0
    je __kefirrt_alloca_noreset
    xor r12, r12
    mov [r14 - 16], r12
__kefirrt_alloca_noreset:
    pop r12             # Alignment
    pop r13             # Size
    mov rsi, [r14 - 8]  # Current stack base
    mov rax, rsi        # Pointer to allocation
    sub rax, r13
    and rax, -8
    cmp r12, 0
    je __kefirrt_alloca_aligned
    neg r12
    and rax, r12
__kefirrt_alloca_aligned:
    mov rcx, rsi        # Count
    sub rcx, rsp
    mov rdi, rax
    sub rdi, rcx
    mov rsi, rsp
    mov rsp, rdi
    cld
    rep movsb
    mov [r14 - 8], rax
    push rax
    end_opcode

define_opcode pushscope
    mov r12, [r14 - 16]
    push r12
    mov r12, [r14 - 8]
    mov [r14 - 16], r12
    end_opcode

define_opcode popscope
    pop rax
    mov rdi, [r14 - 16]
    cmp rdi, 0
    je __kefirrt_popscope_end
    mov rsi, [r14 - 8]
    mov rcx, rsi
    sub rcx, rsp
    dec rsi
    dec rdi
    std
    rep movsb
    cld
    mov r12, [r14 - 16]
    lea rsp, [rdi + 1]
    mov [r14 - 8], r12
    mov [r14 - 16], rax
__kefirrt_popscope_end:
    end_opcode

# Runtime helpers
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

__kefirrt_generic_prologue:
    pop r11
    mov r14, rsp
    sub rsp, 16
    mov [r14 - 8], rsp
    xor r12, r12
    mov [r14 - 16], r12
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
    movaps  [r12 + 48], xmm0
    movaps  [r12 + 64], xmm1
    movaps  [r12 + 80], xmm2
    movaps  [r12 + 96], xmm3
    movaps  [r12 + 112], xmm4
    movaps  [r12 + 128], xmm5
    movaps  [r12 + 144], xmm6
    movaps  [r12 + 160], xmm7
__kefirrt_save_int_registers:
    mov     [r12], rdi
    mov     [r12 + 8], rsi
    mov     [r12 + 16], rdx
    mov     [r12 + 24], rcx
    mov     [r12 + 32], r8
    mov     [r12 + 40], r9
    ret

__kefirrt_load_integer_vararg:
    mov eax, [DATA_REG]
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
    mov eax, [DATA_REG + 4]
    cmp eax, 176
    jae __kefirrt_load_sse_vararg_stack
    lea edx, [eax + 16]
    add rax, [DATA_REG + 2*8]
    mov [DATA_REG + 4], edx
    movaps xmm0, [rax]
    ret
__kefirrt_load_sse_vararg_stack:
    mov rax, [DATA_REG + 8]
    lea rdx, [rax + 8]
    mov [DATA_REG + 8], rdx
    movlps xmm0, [rax]
    ret

__kefirrt_load_long_double_vararg:
    pop DATA_REG
    mov rax, [DATA_REG + 8]
    lea rdx, [rax + 16]
    mov [DATA_REG + 8], rdx
    mov DATA_REG, [rax + 8]
    push DATA_REG
    mov DATA_REG, [rax]
    push DATA_REG
    end_opcode

__kefirrt_copy_vararg:
    pop rsi
    pop rdi
    movsq
    movsq
    movsq
    end_opcode
