#ifndef KEFIR_CODEGEN_AMD64_NATIVEGEN_H_
#define KEFIR_CODEGEN_AMD64_NATIVEGEN_H_

#include <stdio.h>
#include "kefir/core/basic-types.h"
#include "kefir/core/util.h"

typedef enum kefir_amd64_asmgen_datawidth {
    KEFIR_AMD64_QUAD = 'q'
} kefir_amd64_asmgen_datawidth_t;

typedef struct kefir_amd64_asmgen {
    kefir_result_t (*newline)(struct kefir_amd64_asmgen *, unsigned int);
    kefir_result_t (*comment)(struct kefir_amd64_asmgen *, const char *);
    kefir_result_t (*label)(struct kefir_amd64_asmgen *, const char *);
    kefir_result_t (*global)(struct kefir_amd64_asmgen *, const char *);
    kefir_result_t (*external)(struct kefir_amd64_asmgen *, const char *);
    kefir_result_t (*section)(struct kefir_amd64_asmgen *, const char *);
    kefir_result_t (*instr)(struct kefir_amd64_asmgen *, const char *, unsigned int, ...);
    kefir_result_t (*rawdata)(struct kefir_amd64_asmgen *, kefir_amd64_asmgen_datawidth_t, unsigned int, ...);
    kefir_result_t (*close)(struct kefir_amd64_asmgen *);

    void *data;
} kefir_amd64_asmgen_t;

kefir_result_t kefir_amd64_nasm_gen_init(struct kefir_amd64_asmgen *, FILE *);

#define KEFIR_AMD64_ASMGEN_NEWLINE(asmgen, count) \
    ((asmgen)->newline((asmgen), (count)))
#define KEFIR_AMD64_ASMGEN_COMMENT(asmgen, identifier) \
    ((asmgen)->comment((asmgen), (identifier)))
#define KEFIR_AMD64_ASMGEN_LABEL(asmgen, identifier) \
    ((asmgen)->label((asmgen), (identifier)))
#define KEFIR_AMD64_ASMGEN_GLOBAL(asmgen, identifier) \
    ((asmgen)->global((asmgen), (identifier)))
#define KEFIR_AMD64_ASMGEN_EXTERNAL(asmgen, identifier) \
    ((asmgen)->external((asmgen), (identifier)))
#define KEFIR_AMD64_ASMGEN_SECTION(asmgen, identifier) \
    ((asmgen)->section((asmgen), (identifier)))
#define KEFIR_AMD64_ASMGEN_INSTR0(asmgen, opcode) \
    ((asmgen)->instr((asmgen), (opcode), 0))
#define KEFIR_AMD64_ASMGEN_INSTR1(asmgen, opcode, param) \
    ((asmgen)->instr((asmgen), (opcode), 1, (param)))
#define KEFIR_AMD64_ASMGEN_INSTR2(asmgen, opcode, param1, param2) \
    ((asmgen)->instr((asmgen), (opcode), 2, (param1), (param2)))
#define KEFIR_AMD64_ASMGEN_INSTR3(asmgen, opcode, param1, param2, param3) \
    ((asmgen)->instr((asmgen), (opcode), 3, (param1), (param2), (param3)))
#define KEFIR_AMD64_ASMGEN_RAW1(asmgen, width, param) \
    ((asmgen)->rawdata((asmgen), (width), 1, (param)))
#define KEFIR_AMD64_ASMGEN_RAW2(asmgen, width, param1, param2) \
    ((asmgen)->rawdata((asmgen), (width), 2, (param1), (param2)))
#define KEFIR_AMD64_ASMGEN_CLOSE(asmgen) \
    ((asmgen)->close((asmgen)))

#define KEFIR_AMD64_RAX "rax"
#define KEFIR_AMD64_RBX "rbx"
#define KEFIR_AMD64_RCX "rcx"
#define KEFIR_AMD64_RDX "rdx"
#define KEFIR_AMD64_RSI "rsi"
#define KEFIR_AMD64_RDI "rdi"
#define KEFIR_AMD64_RSP "rsp"
#define KEFIR_AMD64_RBP "rbp"
#define KEFIR_AMD64_R8  "r8"
#define KEFIR_AMD64_R9  "r9"
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

#define KEFIR_AMD64_PUSH  "push"
#define KEFIR_AMD64_POP   "pop"
#define KEFIR_AMD64_MOV   "mov"
#define KEFIR_AMD64_LEA   "lea"
#define KEFIR_AMD64_MOVQ  "movq"
#define KEFIR_AMD64_JMP   "jmp"
#define KEFIR_AMD64_RET   "ret"
#define KEFIR_AMD64_FSTCW "fstcw"
#define KEFIR_AMD64_CALL  "call"
#define KEFIR_AMD64_PEXTRQ  "pextrq"
#define KEFIR_AMD64_ADD  "add"
#define KEFIR_AMD64_SUB  "sub"

#define KEFIR_AMD64_INDIRECT(buf, len, content) \
    (kefir_format((buf), (len), "[%s]", (content)))

#endif