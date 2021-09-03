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
#include "kefir/parser/lexer.h"
#include "kefir/parser/format.h"
#include "kefir/ast/format.h"
#include <stdio.h>

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    const char CONTENT[] = "1000 256u 16U       50338l\t\t8273L  \t\v100500uL100373ul 50Lu 150lu\n"
                           "7058462ll\f5846268LL 58270ull\r\r\r\n 1627829Ull         \t\t\t\t 84782llU 1LLu 2LLU\r\n"
                           "0 01\v071        067U 0837l077L 0001LLu\n\n\n"
                           "0x1 0xffe           0x8748\f\f\f0x7U 0x827LL \t\t     0x73lu 0x726Ul0xffLLU\n\n\n\n\r";

    struct kefir_symbol_table symbols;
    struct kefir_lexer_source_cursor cursor;
    struct kefir_parser_integral_types integral_types;
    struct kefir_lexer lexer;
    REQUIRE_OK(kefir_symbol_table_init(&symbols));
    REQUIRE_OK(kefir_lexer_source_cursor_init(&cursor, CONTENT, sizeof(CONTENT), ""));
    REQUIRE_OK(kefir_parser_integral_types_default(&integral_types));
    REQUIRE_OK(kefir_lexer_init(mem, &lexer, &symbols, &cursor, &integral_types));

    struct kefir_json_output json;
    REQUIRE_OK(kefir_json_output_init(&json, stdout, 4));
    REQUIRE_OK(kefir_json_output_array_begin(&json));

    kefir_bool_t scan_tokens = true;
    while (scan_tokens) {
        struct kefir_token token;
        REQUIRE_OK(kefir_lexer_next(mem, &lexer, &token));
        REQUIRE_OK(kefir_token_format(&json, &token));
        scan_tokens = token.klass != KEFIR_TOKEN_SENTINEL;
        REQUIRE_OK(kefir_token_free(mem, &token));
    }

    REQUIRE_OK(kefir_json_output_array_end(&json));
    REQUIRE_OK(kefir_json_output_finalize(&json));

    REQUIRE_OK(kefir_lexer_free(mem, &lexer));
    REQUIRE_OK(kefir_symbol_table_free(mem, &symbols));
    return KEFIR_OK;
}
