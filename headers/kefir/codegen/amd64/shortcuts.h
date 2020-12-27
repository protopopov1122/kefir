#ifndef KEFIR_CODEGEN_AMD64_SHORTCUTS_H_
#define KEFIR_CODEGEN_AMD64_SHORTCUTS_H_

#include "kefir/core/util.h"
#include "kefir/codegen/amd64/asmgen.h"

#define FORMAT(buf, format, ...) (kefir_format((buf), KEFIR_CODEGEN_AMD64_BUFLEN, (format), __VA_ARGS__))
#define INDIRECT(buf, content) KEFIR_AMD64_INDIRECT((buf), KEFIR_CODEGEN_AMD64_BUFLEN, (content))
#define REP(buf, content) KEFIR_AMD64_REP((buf), KEFIR_CODEGEN_AMD64_BUFLEN, (content))
#define ASMGEN_NEWLINE(asmgen, count) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_NEWLINE((asmgen), (count)))
#define ASMGEN_COMMENT(asmgen, identifier) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_COMMENT((asmgen), (identifier)))
#define ASMGEN_LABEL(asmgen, identifier) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_LABEL((asmgen), (identifier)))
#define ASMGEN_GLOBAL(asmgen, identifier) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_GLOBAL((asmgen), (identifier)))
#define ASMGEN_EXTERNAL(asmgen, identifier) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_EXTERNAL((asmgen), (identifier)))
#define ASMGEN_SECTION(asmgen, identifier) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_SECTION((asmgen), (identifier)))
#define ASMGEN_INSTR0(asmgen, opcode) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_INSTR0((asmgen), (opcode)))
#define ASMGEN_INSTR1(asmgen, opcode, param) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_INSTR1((asmgen), (opcode), (param)))
#define ASMGEN_INSTR2(asmgen, opcode, param1, param2) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_INSTR2((asmgen), (opcode), (param1), (param2)))
#define ASMGEN_INSTR3(asmgen, opcode, param1, param2, param3) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_INSTR3((asmgen), (opcode), (param1), (param2), (param3)))
#define ASMGEN_RAW1(asmgen, width, param) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_RAW1((asmgen), (width), (param)))
#define ASMGEN_RAW2(asmgen, width, param1, param2) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_RAW2((asmgen), (width), (param1), (param2)))

#endif