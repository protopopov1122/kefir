/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KEFIR_CODEGEN_AMD64_NATIVEGEN_H_
#define KEFIR_CODEGEN_AMD64_NATIVEGEN_H_

#include <stdbool.h>
#include <stdio.h>
#include "kefir/core/basic-types.h"
#include "kefir/core/util.h"

typedef enum kefir_amd64_asmgen_datawidth {
    KEFIR_AMD64_BYTE = 'b',
    KEFIR_AMD64_WORD = 'w',
    KEFIR_AMD64_DOUBLE = 'd',
    KEFIR_AMD64_QUAD = 'q',
    KEFIR_AMD64_ASCIi = 'a'
} kefir_amd64_asmgen_datawidth_t;

typedef struct kefir_amd64_asmgen {
    kefir_result_t (*prologue)(struct kefir_amd64_asmgen *);
    kefir_result_t (*newline)(struct kefir_amd64_asmgen *, unsigned int);
    kefir_result_t (*comment)(struct kefir_amd64_asmgen *, const char *, ...);
    kefir_result_t (*label)(struct kefir_amd64_asmgen *, const char *, ...);
    kefir_result_t (*global)(struct kefir_amd64_asmgen *, const char *, ...);
    kefir_result_t (*external)(struct kefir_amd64_asmgen *, const char *);
    kefir_result_t (*section)(struct kefir_amd64_asmgen *, const char *);
    kefir_result_t (*instr)(struct kefir_amd64_asmgen *, const char *);
    kefir_result_t (*rawdata)(struct kefir_amd64_asmgen *, kefir_amd64_asmgen_datawidth_t);
    kefir_result_t (*mulrawdata)(struct kefir_amd64_asmgen *, kefir_size_t, kefir_amd64_asmgen_datawidth_t);
    kefir_result_t (*argument)(struct kefir_amd64_asmgen *, const char *, ...);
    kefir_result_t (*string_literal)(struct kefir_amd64_asmgen *, const char *, kefir_size_t);
    kefir_result_t (*close)(struct kefir_amd64_asmgen *);

    void *data;
    struct {
        bool empty;
        kefir_size_t arguments;
    } state;
    struct {
        bool enable_comments;
        bool enable_identation;
    } settings;
} kefir_amd64_asmgen_t;

kefir_result_t kefir_amd64_gas_gen_init(struct kefir_amd64_asmgen *, FILE *);

#define KEFIR_AMD64_ASMGEN_PROLOGUE(asmgen) ((asmgen)->prologue((asmgen)))
#define KEFIR_AMD64_ASMGEN_NEWLINE(asmgen, count) ((asmgen)->newline((asmgen), (count)))
#define KEFIR_AMD64_ASMGEN_COMMENT(asmgen, format, ...) ((asmgen)->comment((asmgen), (format), __VA_ARGS__))
#define KEFIR_AMD64_ASMGEN_LABEL(asmgen, format, ...) ((asmgen)->label((asmgen), (format), __VA_ARGS__))
#define KEFIR_AMD64_ASMGEN_GLOBAL(asmgen, format, ...) ((asmgen)->global((asmgen), (format), __VA_ARGS__))
#define KEFIR_AMD64_ASMGEN_EXTERNAL(asmgen, identifier) ((asmgen)->external((asmgen), (identifier)))
#define KEFIR_AMD64_ASMGEN_SECTION(asmgen, identifier) ((asmgen)->section((asmgen), (identifier)))
#define KEFIR_AMD64_ASMGEN_INSTR(asmgen, opcode) ((asmgen)->instr((asmgen), (opcode)))
#define KEFIR_AMD64_ASMGEN_ARG(asmgen, format, ...) ((asmgen)->argument((asmgen), (format), __VA_ARGS__))
#define KEFIR_AMD64_ASMGEN_RAWDATA(asmgen, width) ((asmgen)->rawdata((asmgen), (width)))
#define KEFIR_AMD64_ASMGEN_STRING_LITERAL(asmgen, literal, length) \
    ((asmgen)->string_literal((asmgen), (literal), (length)))
#define KEFIR_AMD64_ASMGEN_MULRAWDATA(asmgen, times, width) ((asmgen)->mulrawdata((asmgen), (times), (width)))
#define KEFIR_AMD64_ASMGEN_CLOSE(asmgen) ((asmgen)->close((asmgen)))

#define KEFIR_AMD64_EAX "eax"

#define KEFIR_AMD64_RAX "rax"
#define KEFIR_AMD64_RBX "rbx"
#define KEFIR_AMD64_RCX "rcx"
#define KEFIR_AMD64_RDX "rdx"
#define KEFIR_AMD64_RSI "rsi"
#define KEFIR_AMD64_RDI "rdi"
#define KEFIR_AMD64_RSP "rsp"
#define KEFIR_AMD64_RBP "rbp"
#define KEFIR_AMD64_R8 "r8"
#define KEFIR_AMD64_R9 "r9"
#define KEFIR_AMD64_R10 "r10"
#define KEFIR_AMD64_R11 "r11"
#define KEFIR_AMD64_R12 "r12"
#define KEFIR_AMD64_R13 "r13"
#define KEFIR_AMD64_R14 "r14"
#define KEFIR_AMD64_R15 "r15"

#define KEFIR_AMD64_XMM0 "xmm0"
#define KEFIR_AMD64_XMM1 "xmm1"
#define KEFIR_AMD64_XMM2 "xmm2"
#define KEFIR_AMD64_XMM3 "xmm3"
#define KEFIR_AMD64_XMM4 "xmm4"
#define KEFIR_AMD64_XMM5 "xmm5"
#define KEFIR_AMD64_XMM6 "xmm6"
#define KEFIR_AMD64_XMM7 "xmm7"

#define KEFIR_AMD64_FS "fs:"

#define KEFIR_AMD64_PUSH "push"
#define KEFIR_AMD64_POP "pop"
#define KEFIR_AMD64_MOV "mov"
#define KEFIR_AMD64_OR "or"
#define KEFIR_AMD64_LEA "lea"
#define KEFIR_AMD64_MOVQ "movq"
#define KEFIR_AMD64_MOVSB "movsb"
#define KEFIR_AMD64_JMP "jmp"
#define KEFIR_AMD64_JA "ja"
#define KEFIR_AMD64_RET "ret"
#define KEFIR_AMD64_FSTCW "fstcw"
#define KEFIR_AMD64_CALL "call"
#define KEFIR_AMD64_PEXTRQ "pextrq"
#define KEFIR_AMD64_PINSRQ "pinsrq"
#define KEFIR_AMD64_ADD "add"
#define KEFIR_AMD64_CMP "cmp"
#define KEFIR_AMD64_SUB "sub"
#define KEFIR_AMD64_CLD "cld"
#define KEFIR_AMD64_AND "and"
#define KEFIR_AMD64_SHL "shl"
#define KEFIR_AMD64_SHR "shr"
#define KEFIR_AMD64_MOVD "movd"
#define KEFIR_AMD64_FSTP "fstp"
#define KEFIR_AMD64_FLD "fld"

#define KEFIR_AMD64_REP "rep "
#define KEFIR_AMD64_INDIRECT "[%s]"
#define KEFIR_AMD64_INDIRECT_OFFSET "[%s + " KEFIR_SIZE_FMT "]"
#define KEFIR_AMD64_INDIRECT_NEG_OFFSET "[%s - " KEFIR_SIZE_FMT "]"
#define KEFIR_AMD64_ALIGN ".align"
#define KEFIR_AMD64_THREAD_LOCAL "[%s@tpoff]"
#define KEFIR_AMD64_THREAD_LOCAL_GOT "%s@gottpoff[rip]"

#define KEFIR_AMD64_DWORD "DWORD PTR "
#define KEFIR_AMD64_QWORD "QWORD PTR "
#define KEFIR_AMD64_TBYTE "TBYTE PTR "

#endif
