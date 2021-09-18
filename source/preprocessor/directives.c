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

#include "kefir/preprocessor/directives.h"
#include "kefir/preprocessor/tokenizer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t scan_directive(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                     struct kefir_preprocessor_directive *directive) {
    UNUSED(mem);
    UNUSED(preprocessor);
    UNUSED(directive);
    return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Preprocessing directives are not implemented yet");
}

kefir_result_t kefir_preprocessor_next_directive(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                                 struct kefir_preprocessor_directive *directive) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));
    REQUIRE(directive != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor directive"));

    struct kefir_token token;
    struct kefir_token_buffer buffer;
    kefir_bool_t hit_sentinel = false;
    kefir_bool_t whitespaces = true;
    kefir_bool_t continue_scan = true;
    kefir_bool_t found_directive = false;
    REQUIRE_OK(kefir_token_buffer_init(mem, &buffer));
    while (continue_scan) {
        kefir_result_t res = kefir_preprocessor_tokenize_next(mem, &preprocessor->lexer, &token);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_buffer_free(mem, &buffer);
            return res;
        });

        if (token.klass == KEFIR_TOKEN_SENTINEL) {
            continue_scan = false;
            hit_sentinel = true;
            REQUIRE_OK(kefir_token_free(mem, &token));
        } else if (token.klass == KEFIR_TOKEN_PP_WHITESPACE && token.pp_whitespace.newline) {
            continue_scan = false;
            res = kefir_token_buffer_emplace(mem, &buffer, &token);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_token_free(mem, &token);
                kefir_token_buffer_free(mem, &buffer);
                return res;
            });
        } else if (whitespaces && token.klass == KEFIR_TOKEN_PUNCTUATOR && token.punctuator == KEFIR_PUNCTUATOR_HASH) {
            REQUIRE_OK(kefir_token_buffer_free(mem, &buffer));
            REQUIRE_OK(kefir_token_free(mem, &token));
            REQUIRE_OK(scan_directive(mem, preprocessor, directive));
            found_directive = true;
            continue_scan = false;
        } else {
            whitespaces = whitespaces && token.klass == KEFIR_TOKEN_PP_WHITESPACE;
            res = kefir_token_buffer_emplace(mem, &buffer, &token);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_token_free(mem, &token);
                kefir_token_buffer_free(mem, &buffer);
                return res;
            });
        }
    }

    if (hit_sentinel && buffer.length == 0) {
        REQUIRE_OK(kefir_token_buffer_free(mem, &buffer));
        directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_SENTINEL;
    } else if (!found_directive) {
        directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_TEXT_LINE;
        kefir_result_t res = kefir_token_buffer_init(mem, &directive->pp_tokens.pp_tokens);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_buffer_free(mem, &buffer);
            return res;
        });
        res = kefir_token_buffer_insert(mem, &directive->pp_tokens.pp_tokens, &buffer);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_buffer_free(mem, &directive->pp_tokens.pp_tokens);
            kefir_token_buffer_free(mem, &buffer);
            return res;
        });
    }
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_directive_free(struct kefir_mem *mem,
                                                 struct kefir_preprocessor_directive *directive) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(directive != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor directive"));

    switch (directive->type) {
        case KEFIR_PREPROCESSOR_DIRECTIVE_IF:
        case KEFIR_PREPROCESSOR_DIRECTIVE_ELIF:
            REQUIRE_OK(kefir_token_buffer_free(mem, &directive->if_directive.condition));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_IFDEF:
        case KEFIR_PREPROCESSOR_DIRECTIVE_IFNDEF:
            KEFIR_FREE(mem, (void *) directive->ifdef_directive.identifier);
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_INCLUDE:
        case KEFIR_PREPROCESSOR_DIRECTIVE_LINE:
        case KEFIR_PREPROCESSOR_DIRECTIVE_ERROR:
        case KEFIR_PREPROCESSOR_DIRECTIVE_PRAGMA:
        case KEFIR_PREPROCESSOR_DIRECTIVE_NON:
        case KEFIR_PREPROCESSOR_DIRECTIVE_TEXT_LINE:
            REQUIRE_OK(kefir_token_buffer_free(mem, &directive->pp_tokens.pp_tokens));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_DEFINE_OBJECT:
            KEFIR_FREE(mem, (void *) directive->define_obj_directive.identifier);
            REQUIRE_OK(kefir_token_buffer_free(mem, &directive->define_obj_directive.replacement));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_DEFINE_FUNCTION:
            KEFIR_FREE(mem, (void *) directive->define_obj_directive.identifier);
            REQUIRE_OK(kefir_list_free(mem, &directive->define_fun_directive.parameters));
            REQUIRE_OK(kefir_token_buffer_free(mem, &directive->define_obj_directive.replacement));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_UNDEF:
            KEFIR_FREE(mem, (void *) directive->undef_directive.identifier);
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_ELSE:
        case KEFIR_PREPROCESSOR_DIRECTIVE_ENDIF:
        case KEFIR_PREPROCESSOR_DIRECTIVE_EMPTY:
        case KEFIR_PREPROCESSOR_DIRECTIVE_SENTINEL:
            // Intentionally left blank
            break;
    }
    return KEFIR_OK;
}
