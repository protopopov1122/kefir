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

#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/preprocessor/virtual_source_file.h"
#include "kefir/preprocessor/preprocessor.h"
#include "kefir/preprocessor/format.h"
#include <stdio.h>

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    const char CONTENT[] = "#define SUM(x, y) (x) + (y)\n"
                           "SUM(100, 200)\n"
                           "#      define MUL(x\t\t, \vy)(x)*\t\t\t(y)\n"
                           "MUL(SUM(1, 2), 3)\n"
                           "#define    \t FAKE (x, y) x\v           - \t\ty\n"
                           "SUM MUL FAKE(1, 2)\n"
                           "#define STR(...) #__VA_LIST__\n"
                           "#define PRINTF(fmt    , ...) printf(fmt, __VA_LIST__)\n"
                           "STR(100 + 200,     test,\n't') PRINTF(\"TEST\", 1, 2, 3, 400000)\n"
                           "#define     CONCAT(    arg1,    arg2)\t arg1##arg2\n"
                           "    CONCAT(     1,     2000)  CONCAT(,) CONCAT(for, int)";

    struct kefir_symbol_table symbols;
    struct kefir_lexer_context parser_context;
    struct kefir_token_buffer tokens;
    REQUIRE_OK(kefir_symbol_table_init(&symbols));
    REQUIRE_OK(kefir_lexer_context_default(&parser_context));
    REQUIRE_OK(kefir_token_buffer_init(&tokens));

    struct kefir_preprocessor_virtual_source_locator virtual_source;
    struct kefir_preprocessor_context context;
    struct kefir_preprocessor preprocessor;
    struct kefir_lexer_source_cursor cursor;
    REQUIRE_OK(kefir_preprocessor_virtual_source_locator_init(&virtual_source));
    REQUIRE_OK(kefir_preprocessor_context_init(&context, &virtual_source.locator));

    REQUIRE_OK(kefir_lexer_source_cursor_init(&cursor, CONTENT, sizeof(CONTENT), ""));
    REQUIRE_OK(kefir_preprocessor_init(mem, &preprocessor, &symbols, &cursor, &parser_context, &context));
    REQUIRE_OK(kefir_preprocessor_run(mem, &preprocessor, &tokens));
    REQUIRE_OK(kefir_preprocessor_free(mem, &preprocessor));
    REQUIRE_OK(kefir_preprocessor_context_free(mem, &context));
    REQUIRE_OK(kefir_preprocessor_virtual_source_locator_free(mem, &virtual_source));

    REQUIRE_OK(kefir_preprocessor_format(stdout, &tokens, KEFIR_PREPROCESSOR_WHITESPACE_FORMAT_ORIGINAL));
    REQUIRE_OK(kefir_token_buffer_free(mem, &tokens));
    REQUIRE_OK(kefir_symbol_table_free(mem, &symbols));
    return KEFIR_OK;
}
