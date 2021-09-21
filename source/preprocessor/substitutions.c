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

static kefir_result_t substitute_object_macro(struct kefir_mem *mem, struct kefir_preprocessor_token_sequence *seq,
                                              const struct kefir_preprocessor_macro *macro) {
    struct kefir_token_buffer subst_buf;
    REQUIRE_OK(kefir_token_buffer_init(&subst_buf));
    kefir_result_t res = macro->apply(mem, macro, NULL, &subst_buf);
    REQUIRE_CHAIN(&res, kefir_preprocessor_token_sequence_push_front(mem, seq, &subst_buf));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &subst_buf);
        return res;
    });
    REQUIRE_OK(kefir_token_buffer_free(mem, &subst_buf));
    return KEFIR_OK;
}

static kefir_result_t substitute_function_macro(struct kefir_mem *mem, struct kefir_preprocessor_token_sequence *seq,
                                                const struct kefir_preprocessor_macro *macro) {
    UNUSED(macro);
    struct kefir_token_buffer buffer;
    REQUIRE_OK(kefir_token_buffer_init(&buffer));

    const struct kefir_token *token = NULL;
    kefir_bool_t skip_whitespaces = true;
    while (skip_whitespaces) {
        kefir_result_t res = kefir_preprocessor_token_sequence_current(mem, seq, &token);
        if (res == KEFIR_ITERATOR_END) {
            res = KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to substitute identifier by function macro");
        }
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_buffer_free(mem, &buffer);
            return res;
        });

        if (token->klass != KEFIR_TOKEN_PP_WHITESPACE) {
            skip_whitespaces = false;
        } else {
            res = kefir_preprocessor_token_sequence_shift(mem, seq, &buffer);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_token_buffer_free(mem, &buffer);
                return res;
            });
        }
    }

    if (token->klass == KEFIR_TOKEN_PUNCTUATOR && token->punctuator == KEFIR_PUNCTUATOR_LEFT_PARENTHESE) {
        REQUIRE_OK(kefir_token_buffer_free(mem, &buffer));
        return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Function macros are not implemented yet");
    } else {
        kefir_result_t res = kefir_preprocessor_token_sequence_push_front(mem, seq, &buffer);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_buffer_free(mem, &buffer);
            return res;
        });
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to substitute identifier by function macro");
    }
}

static kefir_result_t subst_identifier(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                       struct kefir_preprocessor_token_sequence *seq, const char *identifier) {
    const struct kefir_preprocessor_macro *macro = NULL;
    kefir_result_t res = preprocessor->context->macros->locate(preprocessor->context->macros, identifier, &macro);
    if (res != KEFIR_OK) {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Macro not found for specified identifier");
    }

    if (macro->type == KEFIR_PREPROCESSOR_MACRO_OBJECT) {
        REQUIRE_OK(substitute_object_macro(mem, seq, macro));
    } else {
        REQUIRE_OK(substitute_function_macro(mem, seq, macro));
    }
    return KEFIR_OK;
}

static kefir_result_t run_substitutions(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                        struct kefir_preprocessor_token_sequence *seq,
                                        struct kefir_token_buffer *result) {
    kefir_bool_t scan_tokens = true;
    while (scan_tokens) {
        struct kefir_token token;
        kefir_result_t res = kefir_preprocessor_token_sequence_next(mem, seq, &token);
        if (res == KEFIR_ITERATOR_END) {
            scan_tokens = false;
        } else {
            REQUIRE_OK(res);

            kefir_result_t res = KEFIR_NO_MATCH;
            if (token.klass == KEFIR_TOKEN_IDENTIFIER && !token.preprocessor_props.skip_identifier_subst) {
                res = subst_identifier(mem, preprocessor, seq, token.identifier);
                REQUIRE_CHAIN(&res, kefir_token_free(mem, &token));
            }

            if (res == KEFIR_NO_MATCH) {
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
