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

#ifndef KEFIR_CODEGEN_AMD64_SHORTCUTS_H_
#define KEFIR_CODEGEN_AMD64_SHORTCUTS_H_

#include "kefir/core/util.h"
#include "kefir/codegen/amd64/asmgen.h"

#define ASMGEN_PROLOGUE(asmgen) REQUIRE_OK(KEFIR_AMD64_ASMGEN_PROLOGUE((asmgen)))
#define ASMGEN_NEWLINE(asmgen, count) REQUIRE_OK(KEFIR_AMD64_ASMGEN_NEWLINE((asmgen), (count)))
#define ASMGEN_COMMENT(asmgen, format, ...) REQUIRE_OK(KEFIR_AMD64_ASMGEN_COMMENT((asmgen), (format), __VA_ARGS__))
#define ASMGEN_COMMENT0(asmgen, comment) REQUIRE_OK(KEFIR_AMD64_ASMGEN_COMMENT((asmgen), "%s", (comment)))
#define ASMGEN_LABEL(asmgen, format, ...) REQUIRE_OK(KEFIR_AMD64_ASMGEN_LABEL((asmgen), (format), __VA_ARGS__))
#define ASMGEN_GLOBAL(asmgen, format, ...) REQUIRE_OK(KEFIR_AMD64_ASMGEN_GLOBAL((asmgen), (format), __VA_ARGS__))
#define ASMGEN_EXTERNAL(asmgen, identifier) REQUIRE_OK(KEFIR_AMD64_ASMGEN_EXTERNAL((asmgen), (identifier)))
#define ASMGEN_SECTION(asmgen, identifier) REQUIRE_OK(KEFIR_AMD64_ASMGEN_SECTION((asmgen), (identifier)))
#define ASMGEN_INSTR(asmgen, opcode) REQUIRE_OK(KEFIR_AMD64_ASMGEN_INSTR((asmgen), (opcode)))
#define ASMGEN_ARG(asmgen, format, ...) REQUIRE_OK(KEFIR_AMD64_ASMGEN_ARG((asmgen), (format), __VA_ARGS__))
#define ASMGEN_ARG0(asmgen, arg) REQUIRE_OK(KEFIR_AMD64_ASMGEN_ARG((asmgen), "%s", (arg)))
#define ASMGEN_SYMBOL_ARG(asmgen, arg) REQUIRE_OK(KEFIR_AMD64_ASMGEN_SYMBOL_ARG((asmgen), (arg)))
#define ASMGEN_RAW(asmgen, width) REQUIRE_OK(KEFIR_AMD64_ASMGEN_RAWDATA((asmgen), (width)))
#define ASMGEN_STRING_LITERAL(asmgen, literal, length) KEFIR_AMD64_ASMGEN_STRING_LITERAL((asmgen), (literal), (length))
#define ASMGEN_MULRAW(asmgen, times, width) REQUIRE_OK(KEFIR_AMD64_ASMGEN_MULRAWDATA((asmgen), (times), (width)))
#define ASMGEN_ZERODATA(asmgen, count) REQUIRE_OK(KEFIR_AMD64_ASMGEN_ZERODATA((asmgen), (count)))

#endif
