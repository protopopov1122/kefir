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

#include "kefir/preprocessor/preprocessor.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/preprocessor/directives.h"

kefir_result_t kefir_preprocessor_init(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                       struct kefir_symbol_table *symbols, struct kefir_lexer_source_cursor *cursor,
                                       const struct kefir_lexer_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer source cursor"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser context"));

    REQUIRE_OK(kefir_lexer_init(mem, &preprocessor->lexer, symbols, cursor, context));
    kefir_result_t res = kefir_preprocessor_user_macro_scope_init(NULL, &preprocessor->macros);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_lexer_free(mem, &preprocessor->lexer);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_free(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));

    REQUIRE_OK(kefir_preprocessor_user_macro_scope_free(mem, &preprocessor->macros));
    REQUIRE_OK(kefir_lexer_free(mem, &preprocessor->lexer));
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_run(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                      struct kefir_token_buffer *buffer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));

    struct kefir_preprocessor_directive directive;
    kefir_bool_t scan_directives = true;
    do {
        REQUIRE_OK(kefir_preprocessor_next_directive(mem, preprocessor, &directive));
        switch (directive.type) {
            case KEFIR_PREPROCESSOR_DIRECTIVE_IF:
            case KEFIR_PREPROCESSOR_DIRECTIVE_IFDEF:
            case KEFIR_PREPROCESSOR_DIRECTIVE_IFNDEF:
            case KEFIR_PREPROCESSOR_DIRECTIVE_ELIF:
            case KEFIR_PREPROCESSOR_DIRECTIVE_ELSE:
            case KEFIR_PREPROCESSOR_DIRECTIVE_ENDIF:
            case KEFIR_PREPROCESSOR_DIRECTIVE_INCLUDE:
            case KEFIR_PREPROCESSOR_DIRECTIVE_DEFINE_OBJECT:
            case KEFIR_PREPROCESSOR_DIRECTIVE_DEFINE_FUNCTION:
            case KEFIR_PREPROCESSOR_DIRECTIVE_UNDEF:
            case KEFIR_PREPROCESSOR_DIRECTIVE_LINE:
            case KEFIR_PREPROCESSOR_DIRECTIVE_ERROR:
            case KEFIR_PREPROCESSOR_DIRECTIVE_PRAGMA:
            case KEFIR_PREPROCESSOR_DIRECTIVE_EMPTY:
            case KEFIR_PREPROCESSOR_DIRECTIVE_NON:
                // Not implemented
                break;

            case KEFIR_PREPROCESSOR_DIRECTIVE_TEXT_LINE:
                REQUIRE_OK(kefir_token_buffer_insert(mem, buffer, &directive.pp_tokens.pp_tokens));
                break;

            case KEFIR_PREPROCESSOR_DIRECTIVE_SENTINEL:
                scan_directives = false;
                break;
        }

        REQUIRE_OK(kefir_preprocessor_directive_free(mem, &directive));
    } while (scan_directives);

    return KEFIR_OK;
}
