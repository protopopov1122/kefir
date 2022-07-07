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

#include "kefir/preprocessor/user_macro.h"
#include "kefir/preprocessor/preprocessor.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/hashtree.h"
#include "kefir/core/source_error.h"
#include "kefir/preprocessor/format.h"
#include "kefir/preprocessor/token_sequence.h"
#include "kefir/lexer/format.h"
#include <string.h>

static kefir_result_t user_macro_argc(const struct kefir_preprocessor_macro *macro, kefir_size_t *argc_ptr,
                                      kefir_bool_t *vararg_ptr) {
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro"));
    REQUIRE(argc_ptr != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to argument count"));
    REQUIRE(vararg_ptr != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to vararg flag"));
    ASSIGN_DECL_CAST(struct kefir_preprocessor_user_macro *, user_macro, macro->payload);

    REQUIRE(macro->type == KEFIR_PREPROCESSOR_MACRO_FUNCTION,
            KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST, "Unable to retrieve argument count of object macro"));
    *argc_ptr = kefir_list_length(&user_macro->parameters);
    *vararg_ptr = user_macro->vararg;
    return KEFIR_OK;
}

static kefir_result_t build_function_macro_args_tree(struct kefir_mem *mem,
                                                     const struct kefir_preprocessor_user_macro *user_macro,
                                                     const struct kefir_list *args, struct kefir_hashtree *arg_tree) {
    const struct kefir_list_entry *id_iter = kefir_list_head(&user_macro->parameters);
    const struct kefir_list_entry *arg_iter = kefir_list_head(args);
    for (; id_iter != NULL; kefir_list_next(&id_iter), kefir_list_next(&arg_iter)) {
        REQUIRE(arg_iter != NULL,
                KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Provided macro arguments do no match macro parameters"));
        const char *arg_identifier = id_iter->value;
        struct kefir_token_buffer *arg_value = arg_iter->value;

        REQUIRE_OK(kefir_hashtree_insert(mem, arg_tree, (kefir_hashtree_key_t) arg_identifier,
                                         (kefir_hashtree_value_t) arg_value));
    }

    if (user_macro->vararg) {
        REQUIRE(arg_iter != NULL,
                KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Provided macro arguments do no match macro parameters"));
        const struct kefir_token_buffer *arg_value = arg_iter->value;
        REQUIRE_OK(kefir_hashtree_insert(mem, arg_tree, (kefir_hashtree_key_t) "__VA_ARGS__",
                                         (kefir_hashtree_value_t) arg_value));
    }
    return KEFIR_OK;
}

static kefir_result_t fn_macro_parameter_substitution(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                                      const struct kefir_hashtree *arg_tree,
                                                      struct kefir_token_buffer *buffer, const char *identifier) {
    struct kefir_hashtree_node *node;
    kefir_result_t res = kefir_hashtree_at(arg_tree, (kefir_hashtree_key_t) identifier, &node);
    if (res != KEFIR_NOT_FOUND) {
        REQUIRE_OK(res);

        ASSIGN_DECL_CAST(const struct kefir_token_buffer *, arg_buffer, node->value);
        struct kefir_token_buffer subst_buffer;
        REQUIRE_OK(kefir_token_buffer_init(&subst_buffer));
        kefir_result_t res = kefir_token_buffer_copy(mem, &subst_buffer, arg_buffer);
        REQUIRE_CHAIN(&res, kefir_preprocessor_run_substitutions(mem, preprocessor, &subst_buffer,
                                                                 KEFIR_PREPROCESSOR_SUBSTITUTION_NORMAL));
        REQUIRE_CHAIN(&res, kefir_token_buffer_copy(mem, buffer, &subst_buffer));
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_buffer_free(mem, &subst_buffer);
            return res;
        });
        REQUIRE_OK(kefir_token_buffer_free(mem, &subst_buffer));
    } else {
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to substitute function macro parameter");
    }
    return KEFIR_OK;
}

static kefir_result_t fn_macro_parameter_stringification(struct kefir_mem *mem, const struct kefir_hashtree *arg_tree,
                                                         struct kefir_token_buffer *buffer, const char *identifier,
                                                         const struct kefir_source_location *source_location) {
    struct kefir_hashtree_node *node;
    kefir_result_t res = kefir_hashtree_at(arg_tree, (kefir_hashtree_key_t) identifier, &node);
    if (res == KEFIR_NOT_FOUND) {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, source_location,
                                      "Expected # to be followed by macro parameter");
    } else {
        REQUIRE_OK(res);
    }
    ASSIGN_DECL_CAST(const struct kefir_token_buffer *, arg_buffer, node->value);

    char *string = NULL;
    kefir_size_t string_sz = 0;
    REQUIRE_OK(kefir_preprocessor_format_string(mem, &string, &string_sz, arg_buffer,
                                                KEFIR_PREPROCESSOR_WHITESPACE_FORMAT_SINGLE_SPACE));

    struct kefir_token token;
    res = kefir_token_new_string_literal_multibyte(mem, string, string_sz, &token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, string);
        return res;
    });
    KEFIR_FREE(mem, string);
    token.source_location = *source_location;
    res = kefir_token_buffer_emplace(mem, buffer, &token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &token);
        return res;
    });
    return KEFIR_OK;
}

static const struct kefir_token PlacemakerToken = {.klass = KEFIR_TOKEN_PP_PLACEMAKER};

static kefir_result_t concat_resolve_left_argument(struct kefir_mem *mem, const struct kefir_hashtree *arg_tree,
                                                   struct kefir_token_buffer *buffer, const struct kefir_token *arg,
                                                   const struct kefir_token **result) {
    if (arg->klass == KEFIR_TOKEN_IDENTIFIER && arg_tree != NULL) {
        struct kefir_hashtree_node *node;
        kefir_result_t res = kefir_hashtree_at(arg_tree, (kefir_hashtree_key_t) arg->identifier, &node);
        if (res != KEFIR_NOT_FOUND) {
            REQUIRE_OK(res);
            ASSIGN_DECL_CAST(const struct kefir_token_buffer *, arg_buffer, node->value);

            if (arg_buffer->length == 0) {
                *result = &PlacemakerToken;
            } else {
                struct kefir_token token_copy;
                for (kefir_size_t i = 0; i < arg_buffer->length - 1; i++) {
                    REQUIRE_OK(kefir_token_copy(mem, &token_copy, &arg_buffer->tokens[i]));
                    res = kefir_token_buffer_emplace(mem, buffer, &token_copy);
                    REQUIRE_ELSE(res == KEFIR_OK, {
                        kefir_token_free(mem, &token_copy);
                        return res;
                    });
                }
                *result = &arg_buffer->tokens[arg_buffer->length - 1];
            }
            return KEFIR_OK;
        }
    }

    *result = arg;
    return KEFIR_OK;
}

static kefir_result_t concat_resolve_right_argument(struct kefir_mem *mem, const struct kefir_hashtree *arg_tree,
                                                    struct kefir_token_buffer *buffer, const struct kefir_token *arg,
                                                    const struct kefir_token **result) {
    if (arg->klass == KEFIR_TOKEN_IDENTIFIER && arg_tree != NULL) {
        struct kefir_hashtree_node *node;
        kefir_result_t res = kefir_hashtree_at(arg_tree, (kefir_hashtree_key_t) arg->identifier, &node);
        if (res != KEFIR_NOT_FOUND) {
            REQUIRE_OK(res);
            ASSIGN_DECL_CAST(const struct kefir_token_buffer *, arg_buffer, node->value);

            if (arg_buffer->length == 0) {
                *result = &PlacemakerToken;
            } else {
                struct kefir_token token_copy;
                for (kefir_size_t i = 1; i < arg_buffer->length; i++) {
                    REQUIRE_OK(kefir_token_copy(mem, &token_copy, &arg_buffer->tokens[i]));
                    res = kefir_token_buffer_emplace(mem, buffer, &token_copy);
                    REQUIRE_ELSE(res == KEFIR_OK, {
                        kefir_token_free(mem, &token_copy);
                        return res;
                    });
                }
                *result = &arg_buffer->tokens[0];
            }
            return KEFIR_OK;
        }
    }

    *result = arg;
    return KEFIR_OK;
}

static char *concat_strings(struct kefir_mem *mem, const char *str1, const char *str2) {

    kefir_size_t sz = strlen(str1) + strlen(str2) + 1;
    char *result = KEFIR_MALLOC(mem, sz);
    REQUIRE(result != NULL, NULL);
    strcpy(result, str1);
    strcat(result, str2);
    return result;
}

static kefir_result_t concat_tokens_impl(struct kefir_mem *mem, struct kefir_symbol_table *symbols,
                                         const struct kefir_token *left, const struct kefir_token *right,
                                         struct kefir_token_buffer *buffer) {
    REQUIRE(right->klass == KEFIR_TOKEN_IDENTIFIER || right->klass == KEFIR_TOKEN_PP_NUMBER,
            KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &right->source_location,
                                   "Expected either identifier or preprocessor number"));
    struct kefir_token token;
    kefir_result_t res;
    const char *left_str = left->klass == KEFIR_TOKEN_IDENTIFIER ? left->identifier : left->pp_number.number_literal;
    const char *right_str =
        right->klass == KEFIR_TOKEN_IDENTIFIER ? right->identifier : right->pp_number.number_literal;
    char *result = concat_strings(mem, left_str, right_str);
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate identifier string"));

    if (left->klass == KEFIR_TOKEN_IDENTIFIER) {
        res = kefir_token_new_identifier(mem, symbols, result, &token);
    } else {
        res = kefir_token_new_pp_number(mem, result, strlen(result), &token);
    }
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, result);
        return res;
    });
    token.source_location = left->source_location;
    KEFIR_FREE(mem, result);
    res = kefir_token_buffer_emplace(mem, buffer, &token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &token);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t macro_concat_punctuators(struct kefir_mem *mem, struct kefir_token_buffer *buffer,
                                               kefir_punctuator_token_t punctuator1,
                                               kefir_punctuator_token_t punctuator2) {
    static const struct {
        kefir_punctuator_token_t left;
        kefir_punctuator_token_t right;
        kefir_punctuator_token_t result;
    } Combinations[] = {
        {KEFIR_PUNCTUATOR_MINUS, KEFIR_PUNCTUATOR_GREATER_THAN, KEFIR_PUNCTUATOR_RIGHT_ARROW},
        {KEFIR_PUNCTUATOR_MINUS, KEFIR_PUNCTUATOR_MINUS, KEFIR_PUNCTUATOR_DOUBLE_MINUS},
        {KEFIR_PUNCTUATOR_PLUS, KEFIR_PUNCTUATOR_PLUS, KEFIR_PUNCTUATOR_DOUBLE_PLUS},
        {KEFIR_PUNCTUATOR_LESS_THAN, KEFIR_PUNCTUATOR_LESS_THAN, KEFIR_PUNCTUATOR_LEFT_SHIFT},
        {KEFIR_PUNCTUATOR_GREATER_THAN, KEFIR_PUNCTUATOR_GREATER_THAN, KEFIR_PUNCTUATOR_RIGHT_SHIFT},
        {KEFIR_PUNCTUATOR_LESS_THAN, KEFIR_PUNCTUATOR_ASSIGN, KEFIR_PUNCTUATOR_LESS_OR_EQUAL},
        {KEFIR_PUNCTUATOR_GREATER_THAN, KEFIR_PUNCTUATOR_ASSIGN, KEFIR_PUNCTUATOR_GREATER_OR_EQUAL},
        {KEFIR_PUNCTUATOR_ASSIGN, KEFIR_PUNCTUATOR_ASSIGN, KEFIR_PUNCTUATOR_EQUAL},
        {KEFIR_PUNCTUATOR_EXCLAMATION_MARK, KEFIR_PUNCTUATOR_ASSIGN, KEFIR_PUNCTUATOR_NOT_EQUAL},
        {KEFIR_PUNCTUATOR_AMPERSAND, KEFIR_PUNCTUATOR_AMPERSAND, KEFIR_PUNCTUATOR_DOUBLE_AMPERSAND},
        {KEFIR_PUNCTUATOR_VBAR, KEFIR_PUNCTUATOR_VBAR, KEFIR_PUNCTUATOR_DOUBLE_VBAR},
        {KEFIR_PUNCTUATOR_STAR, KEFIR_PUNCTUATOR_ASSIGN, KEFIR_PUNCTUATOR_ASSIGN_MULTIPLY},
        {KEFIR_PUNCTUATOR_SLASH, KEFIR_PUNCTUATOR_ASSIGN, KEFIR_PUNCTUATOR_ASSIGN_DIVIDE},
        {KEFIR_PUNCTUATOR_PERCENT, KEFIR_PUNCTUATOR_ASSIGN, KEFIR_PUNCTUATOR_ASSIGN_MODULO},
        {KEFIR_PUNCTUATOR_PLUS, KEFIR_PUNCTUATOR_ASSIGN, KEFIR_PUNCTUATOR_ASSIGN_ADD},
        {KEFIR_PUNCTUATOR_MINUS, KEFIR_PUNCTUATOR_ASSIGN, KEFIR_PUNCTUATOR_ASSIGN_SUBTRACT},
        {KEFIR_PUNCTUATOR_LEFT_SHIFT, KEFIR_PUNCTUATOR_ASSIGN, KEFIR_PUNCTUATOR_ASSIGN_SHIFT_LEFT},
        {KEFIR_PUNCTUATOR_LESS_THAN, KEFIR_PUNCTUATOR_LESS_OR_EQUAL, KEFIR_PUNCTUATOR_ASSIGN_SHIFT_LEFT},
        {KEFIR_PUNCTUATOR_RIGHT_SHIFT, KEFIR_PUNCTUATOR_ASSIGN, KEFIR_PUNCTUATOR_ASSIGN_SHIFT_RIGHT},
        {KEFIR_PUNCTUATOR_GREATER_THAN, KEFIR_PUNCTUATOR_GREATER_OR_EQUAL, KEFIR_PUNCTUATOR_ASSIGN_SHIFT_RIGHT},
        {KEFIR_PUNCTUATOR_AMPERSAND, KEFIR_PUNCTUATOR_ASSIGN, KEFIR_PUNCTUATOR_ASSIGN_AND},
        {KEFIR_PUNCTUATOR_CARET, KEFIR_PUNCTUATOR_ASSIGN, KEFIR_PUNCTUATOR_ASSIGN_XOR},
        {KEFIR_PUNCTUATOR_VBAR, KEFIR_PUNCTUATOR_ASSIGN, KEFIR_PUNCTUATOR_ASSIGN_OR},
        {KEFIR_PUNCTUATOR_HASH, KEFIR_PUNCTUATOR_HASH, KEFIR_PUNCTUATOR_DOUBLE_HASH},
        {KEFIR_PUNCTUATOR_LESS_THAN, KEFIR_PUNCTUATOR_COLON, KEFIR_PUNCTUATOR_DIGRAPH_LEFT_BRACKET},
        {KEFIR_PUNCTUATOR_COLON, KEFIR_PUNCTUATOR_GREATER_THAN, KEFIR_PUNCTUATOR_DIGRAPH_RIGHT_BRACKET},
        {KEFIR_PUNCTUATOR_LESS_THAN, KEFIR_PUNCTUATOR_PERCENT, KEFIR_PUNCTUATOR_DIGRAPH_LEFT_BRACE},
        {KEFIR_PUNCTUATOR_PERCENT, KEFIR_PUNCTUATOR_GREATER_THAN, KEFIR_PUNCTUATOR_DIGRAPH_RIGHT_BRACE},
        {KEFIR_PUNCTUATOR_PERCENT, KEFIR_PUNCTUATOR_COLON, KEFIR_PUNCTUATOR_DIGRAPH_HASH},
        {KEFIR_PUNCTUATOR_DIGRAPH_HASH, KEFIR_PUNCTUATOR_DIGRAPH_HASH, KEFIR_PUNCTUATOR_DIGRAPH_DOUBLE_HASH}};
    static const kefir_size_t CombinationCount = sizeof(Combinations) / sizeof(Combinations[0]);
    for (kefir_size_t i = 0; i < CombinationCount; i++) {
        if (Combinations[i].left == punctuator1 && Combinations[i].right == punctuator2) {
            struct kefir_token token;
            REQUIRE_OK(kefir_token_new_punctuator(Combinations[i].result, &token));
            kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &token);
            REQUIRE_ELSE(res == KEFIR_OK, { kefir_token_free(mem, &token); });
            REQUIRE_OK(kefir_token_free(mem, &token));
            return KEFIR_OK;
        }
    }
    return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to concatenate punctuators");
}

static kefir_result_t concat_extension(struct kefir_mem *mem, const struct kefir_token *left,
                                       const struct kefir_token *right, struct kefir_token_buffer *buffer,
                                       const struct kefir_token_extension_class *klass) {
    REQUIRE(klass != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Invalid extension token"));
    struct kefir_token result;
    REQUIRE_OK(klass->concat(mem, left, right, &result));
    kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &result);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_free(mem, &result);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t macro_concatenation_impl(struct kefir_mem *mem, struct kefir_symbol_table *symbols,
                                               struct kefir_token_buffer *buffer, const struct kefir_token *left,
                                               const struct kefir_token *right) {
    if (left->klass == KEFIR_TOKEN_PP_PLACEMAKER) {
        struct kefir_token token;
        REQUIRE_OK(kefir_token_copy(mem, &token, right));
        kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &token);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_free(mem, &token);
            return res;
        });
    } else if (right->klass == KEFIR_TOKEN_PP_PLACEMAKER) {
        struct kefir_token token;
        REQUIRE_OK(kefir_token_copy(mem, &token, left));
        kefir_result_t res = kefir_token_buffer_emplace(mem, buffer, &token);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_free(mem, &token);
            return res;
        });
    } else {
        switch (left->klass) {
            case KEFIR_TOKEN_SENTINEL:
                return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &left->source_location, "Unexpected sentinel token");

            case KEFIR_TOKEN_KEYWORD:
                return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &left->source_location, "Unexpected keyword token");

            case KEFIR_TOKEN_IDENTIFIER:
                if (right->klass != KEFIR_TOKEN_EXTENSION) {
                    REQUIRE_OK(concat_tokens_impl(mem, symbols, left, right, buffer));
                } else {
                    REQUIRE_OK(concat_extension(mem, left, right, buffer, right->extension.klass));
                }
                break;

            case KEFIR_TOKEN_CONSTANT:
                return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &left->source_location, "Unexpected constant token");

            case KEFIR_TOKEN_STRING_LITERAL:
                return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &left->source_location,
                                              "Unexpected string literal token");

            case KEFIR_TOKEN_PUNCTUATOR: {
                if (right->klass != KEFIR_TOKEN_EXTENSION) {
                    REQUIRE(right->klass == KEFIR_TOKEN_PUNCTUATOR,
                            KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &right->source_location,
                                                   "Expected punctuator token"));
                    kefir_result_t res = macro_concat_punctuators(mem, buffer, left->punctuator, right->punctuator);
                    if (res == KEFIR_NO_MATCH) {
                        return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &left->source_location,
                                                      "Unable to concatenate punctuators");
                    } else {
                        REQUIRE_OK(res);
                    }
                } else {
                    REQUIRE_OK(concat_extension(mem, left, right, buffer, right->extension.klass));
                }
            } break;

            case KEFIR_TOKEN_PP_WHITESPACE:
                return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &left->source_location,
                                              "Unexpected preprocessor whitespace token");

            case KEFIR_TOKEN_PP_NUMBER:
                if (right->klass != KEFIR_TOKEN_EXTENSION) {
                    REQUIRE_OK(concat_tokens_impl(mem, symbols, left, right, buffer));
                } else {
                    REQUIRE_OK(concat_extension(mem, left, right, buffer, right->extension.klass));
                }
                break;

            case KEFIR_TOKEN_PP_HEADER_NAME:
                return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &left->source_location,
                                              "Unexpected preprocessor header name token");

            case KEFIR_TOKEN_PP_PLACEMAKER:
                return KEFIR_SET_SOURCE_ERROR(KEFIR_INTERNAL_ERROR, &left->source_location,
                                              "Unexpected placemaker token");

            case KEFIR_TOKEN_EXTENSION:
                REQUIRE_OK(concat_extension(mem, left, right, buffer, left->extension.klass));
                break;
        }
    }
    return KEFIR_OK;
}

static kefir_result_t macro_concatenation(struct kefir_mem *mem, const struct kefir_hashtree *arg_tree,
                                          struct kefir_symbol_table *symbols, struct kefir_token_buffer *buffer,
                                          const struct kefir_token *arg1, const struct kefir_token *arg2) {
    const struct kefir_token *left;
    const struct kefir_token *right;
    struct kefir_token_buffer tail;
    REQUIRE_OK(concat_resolve_left_argument(mem, arg_tree, buffer, arg1, &left));
    REQUIRE_OK(kefir_token_buffer_init(&tail));
    kefir_result_t res = concat_resolve_right_argument(mem, arg_tree, &tail, arg2, &right);
    REQUIRE_CHAIN(&res, macro_concatenation_impl(mem, symbols, buffer, left, right));
    REQUIRE_CHAIN(&res, kefir_token_buffer_insert(mem, buffer, &tail));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &tail);
        return res;
    });
    REQUIRE_OK(kefir_token_buffer_free(mem, &tail));
    return KEFIR_OK;
}

static kefir_result_t match_concatenation(struct kefir_mem *mem, struct kefir_symbol_table *symbols,
                                          struct kefir_preprocessor_token_sequence *seq,
                                          const struct kefir_hashtree *arg_tree, struct kefir_token *left_operand) {
    if (left_operand->klass == KEFIR_TOKEN_PP_WHITESPACE) {
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match preprocessor concatenation");
    }

    const struct kefir_token *next_nonws_token = NULL;
    struct kefir_token_buffer whitespaces;
    REQUIRE_OK(kefir_token_buffer_init(&whitespaces));
    kefir_result_t res = kefir_preprocessor_token_sequence_skip_whitespaces(mem, seq, &next_nonws_token, &whitespaces);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &whitespaces);
        return res;
    });
    if (next_nonws_token == NULL || next_nonws_token->klass != KEFIR_TOKEN_PUNCTUATOR ||
        next_nonws_token->punctuator != KEFIR_PUNCTUATOR_DOUBLE_HASH) {
        res = kefir_preprocessor_token_sequence_push_front(mem, seq, &whitespaces);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_buffer_free(mem, &whitespaces);
            return res;
        });
        REQUIRE_OK(kefir_token_buffer_free(mem, &whitespaces));
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match preprocessor concatenation");
    }
    struct kefir_source_location source_location = next_nonws_token->source_location;
    REQUIRE_OK(kefir_token_buffer_free(mem, &whitespaces));
    REQUIRE_OK(kefir_preprocessor_token_sequence_next(mem, seq, NULL));

    const struct kefir_token *right_operand = NULL;
    REQUIRE_OK(kefir_preprocessor_token_sequence_skip_whitespaces(mem, seq, &right_operand, NULL));
    REQUIRE(
        right_operand != NULL,
        KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &source_location,
                               "Preprocessor concatenation operator cannot be placed at the end of replacement list"));

    struct kefir_token_buffer result;
    REQUIRE_OK(kefir_token_buffer_init(&result));
    res = macro_concatenation(mem, arg_tree, symbols, &result, left_operand, right_operand);
    REQUIRE_CHAIN(&res, kefir_preprocessor_token_sequence_next(mem, seq, NULL));
    REQUIRE_CHAIN(&res, kefir_preprocessor_token_sequence_push_front(mem, seq, &result));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &result);
        return res;
    });
    REQUIRE_OK(kefir_token_buffer_free(mem, &result));
    REQUIRE_OK(kefir_token_free(mem, left_operand));
    return KEFIR_OK;
}

static kefir_result_t run_replacement_list_substitutions_impl(struct kefir_mem *mem,
                                                              struct kefir_preprocessor *preprocessor,
                                                              struct kefir_symbol_table *symbols,
                                                              struct kefir_preprocessor_token_sequence *seq,
                                                              const struct kefir_hashtree *arg_tree,
                                                              struct kefir_token_buffer *result) {
    while (true) {
        struct kefir_token current_token;
        kefir_result_t res = kefir_preprocessor_token_sequence_next(mem, seq, &current_token);
        if (res == KEFIR_ITERATOR_END) {
            break;
        } else {
            REQUIRE_OK(res);
        }

        kefir_bool_t matched = false;
        res = match_concatenation(mem, symbols, seq, arg_tree, &current_token);
        if (res != KEFIR_NO_MATCH) {
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_token_free(mem, &current_token);
                return res;
            });
            matched = true;
        }

        if (!matched && arg_tree != NULL && current_token.klass == KEFIR_TOKEN_IDENTIFIER) {
            res = fn_macro_parameter_substitution(mem, preprocessor, arg_tree, result, current_token.identifier);
            if (res != KEFIR_NO_MATCH) {
                REQUIRE_ELSE(res == KEFIR_OK, {
                    kefir_token_free(mem, &current_token);
                    return res;
                });
                REQUIRE_OK(kefir_token_free(mem, &current_token));
                matched = true;
            }
        }

        if (!matched && arg_tree != NULL && current_token.klass == KEFIR_TOKEN_PUNCTUATOR &&
            current_token.punctuator == KEFIR_PUNCTUATOR_HASH) {
            struct kefir_source_location source_location = current_token.source_location;
            const struct kefir_token *param_token = NULL;
            REQUIRE_OK(kefir_token_free(mem, &current_token));
            REQUIRE_OK(kefir_preprocessor_token_sequence_skip_whitespaces(mem, seq, &param_token, NULL));
            REQUIRE(param_token != NULL && param_token->klass == KEFIR_TOKEN_IDENTIFIER,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &source_location,
                                           "Expected # to be followed by macro parameter"));
            const char *identifier = param_token->identifier;
            REQUIRE_OK(fn_macro_parameter_stringification(mem, arg_tree, result, identifier, &source_location));
            REQUIRE_OK(kefir_preprocessor_token_sequence_next(mem, seq, NULL));
            matched = true;
        }

        if (!matched) {
            res = kefir_token_buffer_emplace(mem, result, &current_token);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_token_free(mem, &current_token);
                return res;
            });
        }
    }
    return KEFIR_OK;
}

static kefir_result_t run_replacement_list_substitutions(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                                         struct kefir_symbol_table *symbols,
                                                         const struct kefir_token_buffer *replacement,
                                                         const struct kefir_hashtree *arg_tree,
                                                         struct kefir_token_buffer *buffer) {

    struct kefir_token_buffer replacement_copy;
    REQUIRE_OK(kefir_token_buffer_init(&replacement_copy));
    kefir_result_t res = kefir_token_buffer_copy(mem, &replacement_copy, replacement);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &replacement_copy);
        return res;
    });

    struct kefir_preprocessor_token_sequence seq;
    res = kefir_preprocessor_token_sequence_init(&seq);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &replacement_copy);
        return res;
    });
    res = kefir_preprocessor_token_sequence_push_front(mem, &seq, &replacement_copy);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_preprocessor_token_sequence_free(mem, &seq);
        kefir_token_buffer_free(mem, &replacement_copy);
        return res;
    });
    res = kefir_token_buffer_free(mem, &replacement_copy);
    REQUIRE_CHAIN(&res, run_replacement_list_substitutions_impl(mem, preprocessor, symbols, &seq, arg_tree, buffer));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_preprocessor_token_sequence_free(mem, &seq);
        return res;
    });

    REQUIRE_OK(kefir_preprocessor_token_sequence_free(mem, &seq));
    return KEFIR_OK;
}

static kefir_result_t apply_function_macro(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                           const struct kefir_preprocessor_user_macro *user_macro,
                                           struct kefir_symbol_table *symbols, const struct kefir_list *args,
                                           struct kefir_token_buffer *buffer) {
    REQUIRE(kefir_list_length(&user_macro->parameters) + (user_macro->vararg ? 1 : 0) <= kefir_list_length(args),
            KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Argument list length mismatch"));
    struct kefir_hashtree arg_tree;
    REQUIRE_OK(kefir_hashtree_init(&arg_tree, &kefir_hashtree_str_ops));

    kefir_result_t res = build_function_macro_args_tree(mem, user_macro, args, &arg_tree);
    REQUIRE_CHAIN(&res, run_replacement_list_substitutions(mem, preprocessor, symbols, &user_macro->replacement,
                                                           &arg_tree, buffer));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_hashtree_free(mem, &arg_tree);
        return res;
    });

    REQUIRE_OK(kefir_hashtree_free(mem, &arg_tree));
    return KEFIR_OK;
}

static kefir_result_t apply_object_macro(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                         const struct kefir_preprocessor_user_macro *user_macro,
                                         struct kefir_symbol_table *symbols, struct kefir_token_buffer *buffer) {
    REQUIRE_OK(run_replacement_list_substitutions(mem, preprocessor, symbols, &user_macro->replacement, NULL, buffer));
    return KEFIR_OK;
}

static kefir_result_t user_macro_apply(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                       const struct kefir_preprocessor_macro *macro, struct kefir_symbol_table *symbols,
                                       const struct kefir_list *args, struct kefir_token_buffer *buffer,
                                       const struct kefir_source_location *source_location) {
    UNUSED(source_location);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));
    ASSIGN_DECL_CAST(struct kefir_preprocessor_user_macro *, user_macro, macro->payload);

    struct kefir_token_buffer inner_buffer;
    kefir_result_t res;
    if (macro->type == KEFIR_PREPROCESSOR_MACRO_FUNCTION) {
        REQUIRE(args != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid macro argument list"));
        REQUIRE_OK(kefir_token_buffer_init(&inner_buffer));
        res = apply_function_macro(mem, preprocessor, user_macro, symbols, args, &inner_buffer);
    } else {
        REQUIRE(args == NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected empty macro argument list"));
        REQUIRE_OK(kefir_token_buffer_init(&inner_buffer));
        res = apply_object_macro(mem, preprocessor, user_macro, symbols, &inner_buffer);
    }
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &inner_buffer);
        return res;
    });

    for (kefir_size_t i = 0; i < inner_buffer.length; i++) {
        struct kefir_token *token = &inner_buffer.tokens[i];
        if (token->klass == KEFIR_TOKEN_IDENTIFIER && strcmp(token->identifier, macro->identifier) == 0) {
            token->preprocessor_props.skip_identifier_subst = true;
        }
        token->source_location = *source_location;

        if (token->klass != KEFIR_TOKEN_PP_PLACEMAKER) {
            res = kefir_token_buffer_emplace(mem, buffer, token);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_token_buffer_free(mem, &inner_buffer);
                return res;
            });
        }
    }
    REQUIRE_OK(kefir_token_buffer_free(mem, &inner_buffer));

    return KEFIR_OK;
}

struct kefir_preprocessor_user_macro *kefir_preprocessor_user_macro_new_object(struct kefir_mem *mem,
                                                                               struct kefir_symbol_table *symbols,
                                                                               const char *identifier) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(identifier != NULL, NULL);

    if (symbols != NULL) {
        identifier = kefir_symbol_table_insert(mem, symbols, identifier, NULL);
        REQUIRE(identifier != NULL, NULL);
    }

    struct kefir_preprocessor_user_macro *macro = KEFIR_MALLOC(mem, sizeof(struct kefir_preprocessor_user_macro));
    REQUIRE(macro != NULL, NULL);

    kefir_result_t res = kefir_token_buffer_init(&macro->replacement);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, macro);
        return NULL;
    });

    macro->macro.identifier = identifier;
    macro->macro.type = KEFIR_PREPROCESSOR_MACRO_OBJECT;
    macro->macro.payload = macro;
    macro->macro.argc = user_macro_argc;
    macro->macro.apply = user_macro_apply;
    return macro;
}

struct kefir_preprocessor_user_macro *kefir_preprocessor_user_macro_new_function(struct kefir_mem *mem,
                                                                                 struct kefir_symbol_table *symbols,
                                                                                 const char *identifier) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(identifier != NULL, NULL);

    if (symbols != NULL) {
        identifier = kefir_symbol_table_insert(mem, symbols, identifier, NULL);
        REQUIRE(identifier != NULL, NULL);
    }

    struct kefir_preprocessor_user_macro *macro = KEFIR_MALLOC(mem, sizeof(struct kefir_preprocessor_user_macro));
    REQUIRE(macro != NULL, NULL);

    kefir_result_t res = kefir_list_init(&macro->parameters);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, macro);
        return NULL;
    });

    res = kefir_token_buffer_init(&macro->replacement);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &macro->parameters);
        KEFIR_FREE(mem, macro);
        return NULL;
    });

    macro->macro.identifier = identifier;
    macro->macro.type = KEFIR_PREPROCESSOR_MACRO_FUNCTION;
    macro->vararg = false;
    macro->macro.payload = macro;
    macro->macro.argc = user_macro_argc;
    macro->macro.apply = user_macro_apply;
    return macro;
}

kefir_result_t kefir_preprocessor_user_macro_free(struct kefir_mem *mem, struct kefir_preprocessor_user_macro *macro) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro"));

    REQUIRE_OK(kefir_token_buffer_free(mem, &macro->replacement));
    if (macro->macro.type == KEFIR_PREPROCESSOR_MACRO_FUNCTION) {
        REQUIRE_OK(kefir_list_free(mem, &macro->parameters));
    }
    macro->vararg = false;
    macro->macro.identifier = NULL;
    macro->macro.payload = NULL;
    macro->macro.argc = NULL;
    macro->macro.apply = NULL;
    KEFIR_FREE(mem, macro);
    return KEFIR_OK;
}

static kefir_result_t free_macro(struct kefir_mem *mem, struct kefir_hashtree *tree, kefir_hashtree_key_t key,
                                 kefir_hashtree_value_t value, void *payload) {
    UNUSED(tree);
    UNUSED(key);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    ASSIGN_DECL_CAST(struct kefir_preprocessor_user_macro *, macro, value);
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro"));

    REQUIRE_OK(kefir_preprocessor_user_macro_free(mem, macro));
    return KEFIR_OK;
}

static kefir_result_t user_macro_locate(const struct kefir_preprocessor_macro_scope *scope, const char *identifier,
                                        const struct kefir_preprocessor_macro **macro_ptr) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid identifier"));
    REQUIRE(macro_ptr != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor macro"));

    ASSIGN_DECL_CAST(struct kefir_preprocessor_user_macro_scope *, user_macro_scope, scope->payload);
    const struct kefir_preprocessor_user_macro *user_macro;
    REQUIRE_OK(kefir_preprocessor_user_macro_scope_at(user_macro_scope, identifier, &user_macro));
    *macro_ptr = &user_macro->macro;
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_user_macro_scope_init(const struct kefir_preprocessor_user_macro_scope *parent,
                                                        struct kefir_preprocessor_user_macro_scope *scope) {
    REQUIRE(scope != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor macro scope"));

    REQUIRE_OK(kefir_hashtree_init(&scope->macros, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_hashtree_on_removal(&scope->macros, free_macro, NULL));
    scope->parent = parent;
    scope->scope.locate = user_macro_locate;
    scope->scope.payload = scope;
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_user_macro_scope_free(struct kefir_mem *mem,
                                                        struct kefir_preprocessor_user_macro_scope *scope) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro scope"));

    REQUIRE_OK(kefir_hashtree_free(mem, &scope->macros));
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_user_macro_scope_insert(struct kefir_mem *mem,
                                                          struct kefir_preprocessor_user_macro_scope *scope,
                                                          struct kefir_preprocessor_user_macro *macro) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro scope"));
    REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro"));

    if (kefir_hashtree_has(&scope->macros, (kefir_hashtree_key_t) macro->macro.identifier)) {
        REQUIRE_OK(kefir_hashtree_delete(mem, &scope->macros, (kefir_hashtree_key_t) macro->macro.identifier));
    }
    REQUIRE_OK(kefir_hashtree_insert(mem, &scope->macros, (kefir_hashtree_key_t) macro->macro.identifier,
                                     (kefir_hashtree_value_t) macro));
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_user_macro_scope_at(const struct kefir_preprocessor_user_macro_scope *scope,
                                                      const char *identifier,
                                                      const struct kefir_preprocessor_user_macro **macro_ptr) {
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid dentifier"));
    REQUIRE(macro_ptr != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor macro"));

    struct kefir_hashtree_node *node = NULL;
    kefir_result_t res = kefir_hashtree_at(&scope->macros, (kefir_hashtree_key_t) identifier, &node);
    if (res == KEFIR_NOT_FOUND && scope->parent != NULL) {
        REQUIRE_OK(kefir_preprocessor_user_macro_scope_at(scope->parent, identifier, macro_ptr));
    } else {
        REQUIRE_OK(res);
        *macro_ptr = (const struct kefir_preprocessor_user_macro *) node->value;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_user_macro_scope_remove(struct kefir_mem *mem,
                                                          struct kefir_preprocessor_user_macro_scope *scope,
                                                          const char *identifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(scope != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor macro scope"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid dentifier"));

    kefir_result_t res = kefir_hashtree_delete(mem, &scope->macros, (kefir_hashtree_key_t) identifier);
    if (res != KEFIR_NOT_FOUND) {
        REQUIRE_OK(res);
    }
    return KEFIR_OK;
}
