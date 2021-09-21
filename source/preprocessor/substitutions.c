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
#include "kefir/preprocessor/token_sequence.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t subst_identifier(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                       struct kefir_preprocessor_token_sequence *seq, struct kefir_token *token,
                                       struct kefir_token_buffer *result) {
    const struct kefir_preprocessor_macro *macro = NULL;
    kefir_result_t res =
        preprocessor->context->macros->locate(preprocessor->context->macros, token->identifier, &macro);
    if (res != KEFIR_OK) {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        REQUIRE_OK(kefir_token_buffer_emplace(mem, result, token));
        return KEFIR_OK;
    }

    if (macro->type == KEFIR_PREPROCESSOR_MACRO_OBJECT) {
        struct kefir_token_buffer subst_buf;
        REQUIRE_OK(kefir_token_buffer_init(&subst_buf));
        kefir_result_t res = macro->apply(mem, macro, NULL, &subst_buf);
        REQUIRE_CHAIN(&res, kefir_preprocessor_token_sequence_push_front(mem, seq, &subst_buf));
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_buffer_free(mem, &subst_buf);
            return res;
        });
        REQUIRE_OK(kefir_token_buffer_free(mem, &subst_buf));
    } else {
        return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Function macros are not implemented yet");
    }
    return KEFIR_OK;
}

static kefir_result_t run_substitutions(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                        struct kefir_preprocessor_token_sequence *seq,
                                        struct kefir_token_buffer *result) {
    UNUSED(preprocessor);
    kefir_bool_t scan_tokens = true;
    while (scan_tokens) {
        struct kefir_token token;
        kefir_result_t res = kefir_preprocessor_token_sequence_next(mem, seq, &token);
        if (res == KEFIR_ITERATOR_END) {
            scan_tokens = false;
        } else {
            REQUIRE_OK(res);

            if (token.klass == KEFIR_TOKEN_IDENTIFIER && !token.preprocessor_props.skip_identifier_subst) {
                res = subst_identifier(mem, preprocessor, seq, &token, result);
                REQUIRE_CHAIN(&res, kefir_token_free(mem, &token));
            } else {
                res = kefir_token_buffer_emplace(mem, result, &token);
            }
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_token_free(mem, &token);
                return res;
            });
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_run_substitutions(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                                    struct kefir_token_buffer *buffer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid tokem buffer"));

    struct kefir_preprocessor_token_sequence seq;
    REQUIRE_OK(kefir_preprocessor_token_sequence_init(&seq));
    kefir_result_t res = kefir_preprocessor_token_sequence_push_front(mem, &seq, buffer);
    REQUIRE_CHAIN(&res, run_substitutions(mem, preprocessor, &seq, buffer));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_preprocessor_token_sequence_free(mem, &seq);
        return res;
    });
    REQUIRE_OK(kefir_preprocessor_token_sequence_free(mem, &seq));
    return KEFIR_OK;
}
