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
#include "kefir/preprocessor/preprocessor.h"
#include "kefir/preprocessor/format.h"
#include <stdio.h>

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    const char CONTENT[] = "int i = 2+2*\t\t2;\n\n\n"
                           "\tfor (int x = 0; x < i; ++x) fn(\'X\', \"xA\\n\");\n"
                           "     \t str = L\"long string\", xstr = U\"UUU\\u1111-uuuu\"";

    struct kefir_symbol_table symbols;
    struct kefir_lexer_source_cursor cursor;
    struct kefir_lexer_context parser_context;
    REQUIRE_OK(kefir_symbol_table_init(&symbols));
    REQUIRE_OK(kefir_lexer_source_cursor_init(&cursor, CONTENT, sizeof(CONTENT), ""));
    REQUIRE_OK(kefir_lexer_context_default(&parser_context));

    struct kefir_preprocessor preprocessor;
    struct kefir_token_buffer tokens;
    REQUIRE_OK(kefir_preprocessor_init(mem, &preprocessor, &symbols, &cursor, &parser_context));
    REQUIRE_OK(kefir_token_buffer_init(mem, &tokens));
    REQUIRE_OK(kefir_preprocessor_run(mem, &preprocessor, &tokens));
    REQUIRE_OK(kefir_preprocessor_format(stdout, &tokens));
    REQUIRE_OK(kefir_preprocessor_free(mem, &preprocessor));

    REQUIRE_OK(kefir_token_buffer_free(mem, &tokens));
    REQUIRE_OK(kefir_symbol_table_free(mem, &symbols));
    return KEFIR_OK;
}
