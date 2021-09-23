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
#include "kefir/core/source_error.h"

static kefir_result_t substitute_object_macro(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                              struct kefir_preprocessor_token_sequence *seq,
                                              const struct kefir_preprocessor_macro *macro) {
    struct kefir_token_buffer subst_buf;
    REQUIRE_OK(kefir_token_buffer_init(&subst_buf));
    kefir_result_t res = macro->apply(mem, macro, preprocessor->lexer.symbols, NULL, &subst_buf);
    REQUIRE_CHAIN(&res, kefir_preprocessor_token_sequence_push_front(mem, seq, &subst_buf));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &subst_buf);
        return res;
    });
    REQUIRE_OK(kefir_token_buffer_free(mem, &subst_buf));
    return KEFIR_OK;
}

static kefir_result_t free_argument(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                    void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid list entry"));

    ASSIGN_DECL_CAST(struct kefir_token_buffer *, buffer, entry->value);
    REQUIRE_OK(kefir_token_buffer_free(mem, buffer));
    KEFIR_FREE(mem, buffer);
    return KEFIR_OK;
}

static kefir_result_t function_macro_argument_buffer_append(struct kefir_mem *mem,
                                                            struct kefir_preprocessor_token_sequence *seq,
                                                            struct kefir_list *args) {
    struct kefir_list_entry *tail = kefir_list_tail(args);
    struct kefir_token_buffer *buffer = NULL;
    if (tail == NULL) {
        buffer = KEFIR_MALLOC(mem, sizeof(struct kefir_token_buffer));
        REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate token buffer"));
        kefir_result_t res = kefir_token_buffer_init(buffer);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, buffer);
            return res;
        });
        res = kefir_list_insert_after(mem, args, tail, buffer);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_buffer_free(mem, buffer);
            KEFIR_FREE(mem, buffer);
            return res;
        });
    } else {
        buffer = tail->value;
    }
    REQUIRE_OK(kefir_preprocessor_token_sequence_shift(mem, seq, buffer));
    return KEFIR_OK;
}

static kefir_result_t function_macro_arguments_push(struct kefir_mem *mem, struct kefir_list *args) {
    struct kefir_token_buffer *buffer = KEFIR_MALLOC(mem, sizeof(struct kefir_token_buffer));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate token buffer"));
    kefir_result_t res = kefir_token_buffer_init(buffer);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, buffer);
        return res;
    });
    res = kefir_list_insert_after(mem, args, kefir_list_tail(args), buffer);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, buffer);
        KEFIR_FREE(mem, buffer);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t prepare_function_macro_argument(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                                      struct kefir_token_buffer *buffer) {
    REQUIRE_OK(kefir_preprocessor_run_substitutions(mem, preprocessor, buffer));
    while (buffer->length > 0 && buffer->tokens[buffer->length - 1].klass == KEFIR_TOKEN_PP_WHITESPACE) {
        REQUIRE_OK(kefir_token_buffer_pop(mem, buffer));
    }
    return KEFIR_OK;
}

static kefir_result_t scan_function_macro_arguments(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                                    struct kefir_preprocessor_token_sequence *seq,
                                                    struct kefir_list *args, kefir_size_t argc, kefir_bool_t vararg) {
    kefir_size_t nested_parens = 0;
    kefir_bool_t scan_args = true;
    const struct kefir_token *token;
    kefir_result_t remaining_args = argc;
    REQUIRE_OK(kefir_preprocessor_token_sequence_next(mem, seq, NULL));
    REQUIRE_OK(kefir_preprocessor_token_sequence_skip_whitespaces(mem, seq, NULL, NULL));

    while (scan_args) {
        kefir_result_t res = kefir_preprocessor_token_sequence_current(mem, seq, &token);
        if (res == KEFIR_ITERATOR_END) {
            return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, NULL, "Unexpected end of file");
        } else {
            REQUIRE_OK(res);
        }

        if (token->klass == KEFIR_TOKEN_PUNCTUATOR && token->punctuator == KEFIR_PUNCTUATOR_LEFT_PARENTHESE) {
            nested_parens++;
            REQUIRE_OK(function_macro_argument_buffer_append(mem, seq, args));
        } else if (token->klass == KEFIR_TOKEN_PUNCTUATOR && token->punctuator == KEFIR_PUNCTUATOR_RIGHT_PARENTHESE) {
            if (nested_parens > 0) {
                nested_parens--;
                REQUIRE_OK(function_macro_argument_buffer_append(mem, seq, args));
            } else {
                scan_args = false;
            }
        } else if (token->klass == KEFIR_TOKEN_PUNCTUATOR && token->punctuator == KEFIR_PUNCTUATOR_COMMA &&
                   nested_parens == 0) {
            if (remaining_args == 0) {
                REQUIRE(vararg, KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, NULL, "Unexpected comma"));
                REQUIRE_OK(function_macro_argument_buffer_append(mem, seq, args));
            } else {
                remaining_args--;
                REQUIRE(remaining_args > 0 || vararg,
                        KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, NULL, "Unexpected comma"));
                if (kefir_list_length(args) == 0) {
                    REQUIRE_OK(function_macro_arguments_push(mem, args));
                }
                REQUIRE_OK(function_macro_arguments_push(mem, args));
                REQUIRE_OK(kefir_preprocessor_token_sequence_next(mem, seq, NULL));
                REQUIRE_OK(kefir_preprocessor_token_sequence_skip_whitespaces(mem, seq, NULL, NULL));
            }
        } else if (token->klass == KEFIR_TOKEN_SENTINEL) {
            scan_args = false;
        } else {
            REQUIRE_OK(function_macro_argument_buffer_append(mem, seq, args));
        }
    }

    REQUIRE(token->klass == KEFIR_TOKEN_PUNCTUATOR && token->punctuator == KEFIR_PUNCTUATOR_RIGHT_PARENTHESE,
            KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &token->source_location, "Expected right parenthese"));
    REQUIRE_OK(kefir_preprocessor_token_sequence_next(mem, seq, NULL));

    if (kefir_list_length(args) == 0 && (argc == 1 || vararg)) {
        REQUIRE_OK(function_macro_arguments_push(mem, args));
    }
    REQUIRE(kefir_list_length(args) == argc + (vararg ? 1 : 0),
            KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, NULL, "Argument count mismatch"));

    for (const struct kefir_list_entry *iter = kefir_list_head(args); iter != NULL; kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_token_buffer *, arg_buffer, iter->value);
        REQUIRE_OK(prepare_function_macro_argument(mem, preprocessor, arg_buffer));
    }
    return KEFIR_OK;
}

static kefir_result_t apply_function_macro(struct kefir_mem *mem, struct kefir_preprocessor_token_sequence *seq,
                                           struct kefir_preprocessor *preprocessor,
                                           const struct kefir_preprocessor_macro *macro,
                                           const struct kefir_list *args) {
    struct kefir_token_buffer subst_buf;
    REQUIRE_OK(kefir_token_buffer_init(&subst_buf));
    kefir_result_t res = macro->apply(mem, macro, preprocessor->lexer.symbols, args, &subst_buf);
    REQUIRE_CHAIN(&res, kefir_preprocessor_token_sequence_push_front(mem, seq, &subst_buf));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &subst_buf);
        return res;
    });
    REQUIRE_OK(kefir_token_buffer_free(mem, &subst_buf));
    return KEFIR_OK;
}

static kefir_result_t substitute_function_macro_impl(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                                     struct kefir_preprocessor_token_sequence *seq,
                                                     const struct kefir_preprocessor_macro *macro) {
    kefir_size_t argc;
    kefir_bool_t vararg;
    REQUIRE_OK(macro->argc(macro, &argc, &vararg));

    struct kefir_list arguments;
    REQUIRE_OK(kefir_list_init(&arguments));
    REQUIRE_OK(kefir_list_on_remove(&arguments, free_argument, NULL));
    kefir_result_t res = scan_function_macro_arguments(mem, preprocessor, seq, &arguments, argc, vararg);
    REQUIRE_CHAIN(&res, apply_function_macro(mem, seq, preprocessor, macro, &arguments));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &arguments);
        return res;
    });
    REQUIRE_OK(kefir_list_free(mem, &arguments));
    return KEFIR_OK;
}

static kefir_result_t substitute_function_macro(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                                struct kefir_preprocessor_token_sequence *seq,
                                                const struct kefir_preprocessor_macro *macro) {
    struct kefir_token_buffer buffer;
    REQUIRE_OK(kefir_token_buffer_init(&buffer));

    const struct kefir_token *token = NULL;
    kefir_result_t res = kefir_preprocessor_token_sequence_skip_whitespaces(mem, seq, &token, &buffer);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &buffer);
        return res;
    });

    if (token != NULL && token->klass == KEFIR_TOKEN_PUNCTUATOR &&
        token->punctuator == KEFIR_PUNCTUATOR_LEFT_PARENTHESE) {
        REQUIRE_OK(kefir_token_buffer_free(mem, &buffer));
        REQUIRE_OK(substitute_function_macro_impl(mem, preprocessor, seq, macro));
    } else {
        kefir_result_t res = kefir_preprocessor_token_sequence_push_front(mem, seq, &buffer);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_buffer_free(mem, &buffer);
            return res;
        });
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match function macro invocation");
    }
    return KEFIR_OK;
}

static kefir_result_t substitute_identifier(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                            struct kefir_preprocessor_token_sequence *seq, const char *identifier) {
    const struct kefir_preprocessor_macro *macro = NULL;
    kefir_result_t res = preprocessor->context->macros->locate(preprocessor->context->macros, identifier, &macro);
    if (res != KEFIR_OK) {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Macro not found for specified identifier");
    }

    if (macro->type == KEFIR_PREPROCESSOR_MACRO_OBJECT) {
        REQUIRE_OK(substitute_object_macro(mem, preprocessor, seq, macro));
    } else {
        REQUIRE_OK(substitute_function_macro(mem, preprocessor, seq, macro));
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
                res = substitute_identifier(mem, preprocessor, seq, token.identifier);
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
