#ifndef KEFIR_CODEGEN_AMD64_SHORTCUTS_H_
#define KEFIR_CODEGEN_AMD64_SHORTCUTS_H_

#include "kefir/core/util.h"
#include "kefir/codegen/amd64/asmgen.h"

#define ASMGEN_NEWLINE(asmgen, count) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_NEWLINE((asmgen), (count)))
#define ASMGEN_COMMENT(asmgen, format, ...) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_COMMENT((asmgen), (format), __VA_ARGS__))
#define ASMGEN_COMMENT0(asmgen, comment) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_COMMENT((asmgen), "%s", (comment)))
#define ASMGEN_LABEL(asmgen, format, ...) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_LABEL((asmgen), (format), __VA_ARGS__))
#define ASMGEN_GLOBAL(asmgen, format, ...) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_GLOBAL((asmgen), (format), __VA_ARGS__))
#define ASMGEN_EXTERNAL(asmgen, identifier) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_EXTERNAL((asmgen), (identifier)))
#define ASMGEN_SECTION(asmgen, identifier) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_SECTION((asmgen), (identifier)))
#define ASMGEN_INSTR(asmgen, opcode) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_INSTR((asmgen), (opcode)))
#define ASMGEN_ARG(asmgen, format, ...) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_ARG((asmgen), (format), __VA_ARGS__))
#define ASMGEN_ARG0(asmgen, arg) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_ARG((asmgen), "%s", (arg)))
#define ASMGEN_RAW(asmgen, width) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_RAWDATA((asmgen), (width)))
#define ASMGEN_MULRAW(asmgen, times, width) \
    REQUIRE_OK(KEFIR_AMD64_ASMGEN_MULRAWDATA((asmgen), (times), (width)))

#endif