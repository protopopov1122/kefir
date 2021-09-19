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
#include "kefir/core/source_error.h"

kefir_result_t kefir_preprocessor_context_init(struct kefir_preprocessor_context *context,
                                               const struct kefir_preprocessor_source_locator *locator) {
    REQUIRE(context != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor context"));
    REQUIRE(locator != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor source locator"));

    REQUIRE_OK(kefir_preprocessor_user_macro_scope_init(NULL, &context->macros));
    context->source_locator = locator;
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_context_free(struct kefir_mem *mem, struct kefir_preprocessor_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor context"));

    REQUIRE_OK(kefir_preprocessor_user_macro_scope_free(mem, &context->macros));
    context->source_locator = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_init(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                       struct kefir_symbol_table *symbols, struct kefir_lexer_source_cursor *cursor,
                                       const struct kefir_lexer_context *context,
                                       struct kefir_preprocessor_context *preprocessor_context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer source cursor"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser context"));
    REQUIRE(preprocessor_context != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor context"));

    REQUIRE_OK(kefir_lexer_init(mem, &preprocessor->lexer, symbols, cursor, context));
    preprocessor->context = preprocessor_context;
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_free(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));

    REQUIRE_OK(kefir_lexer_free(mem, &preprocessor->lexer));
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_skip_line(struct kefir_preprocessor *preprocessor) {
    kefir_bool_t skip = true;
    while (skip) {
        kefir_char32_t chr = kefir_lexer_source_cursor_at(preprocessor->lexer.cursor, 0);
        if (chr == U'\0') {
            skip = false;
        } else {
            skip = chr != preprocessor->lexer.context->newline;
            REQUIRE_OK(kefir_lexer_source_cursor_next(preprocessor->lexer.cursor, 1));
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_skip_group(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));

    kefir_preprocessor_directive_type_t directive;
    kefir_size_t nested_ifs = 0;
    kefir_bool_t skip = true;

    while (skip) {
        struct kefir_lexer_source_cursor_state state;
        REQUIRE_OK(kefir_lexer_source_cursor_save(preprocessor->lexer.cursor, &state));
        REQUIRE_OK(kefir_preprocessor_match_directive(mem, preprocessor, &directive));
        switch (directive) {
            case KEFIR_PREPROCESSOR_DIRECTIVE_IF:
            case KEFIR_PREPROCESSOR_DIRECTIVE_IFDEF:
            case KEFIR_PREPROCESSOR_DIRECTIVE_IFNDEF:
                nested_ifs++;
                REQUIRE_OK(kefir_preprocessor_skip_line(preprocessor));
                break;

            case KEFIR_PREPROCESSOR_DIRECTIVE_ELIF:
            case KEFIR_PREPROCESSOR_DIRECTIVE_ELSE:
            case KEFIR_PREPROCESSOR_DIRECTIVE_ENDIF:
                if (nested_ifs > 0) {
                    nested_ifs--;
                    REQUIRE_OK(kefir_preprocessor_skip_line(preprocessor));
                } else {
                    REQUIRE_OK(kefir_lexer_source_cursor_restore(preprocessor->lexer.cursor, &state));
                    skip = false;
                }
                break;

            case KEFIR_PREPROCESSOR_DIRECTIVE_SENTINEL:
                skip = false;
                break;

            case KEFIR_PREPROCESSOR_DIRECTIVE_INCLUDE:
            case KEFIR_PREPROCESSOR_DIRECTIVE_DEFINE_OBJECT:
            case KEFIR_PREPROCESSOR_DIRECTIVE_DEFINE_FUNCTION:
            case KEFIR_PREPROCESSOR_DIRECTIVE_UNDEF:
            case KEFIR_PREPROCESSOR_DIRECTIVE_LINE:
            case KEFIR_PREPROCESSOR_DIRECTIVE_ERROR:
            case KEFIR_PREPROCESSOR_DIRECTIVE_PRAGMA:
            case KEFIR_PREPROCESSOR_DIRECTIVE_EMPTY:
            case KEFIR_PREPROCESSOR_DIRECTIVE_NON:
            case KEFIR_PREPROCESSOR_DIRECTIVE_TEXT_LINE:
                REQUIRE_OK(kefir_preprocessor_skip_line(preprocessor));
                break;
        }
    }
    return KEFIR_OK;
}

enum if_condition_state { IF_CONDITION_NONE, IF_CONDITION_SUCCESS, IF_CONDITION_FAIL };

static kefir_result_t process_include(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                      struct kefir_token_buffer *buffer,
                                      struct kefir_preprocessor_directive *directive) {
    // TODO Process directive tokens
    REQUIRE(directive->pp_tokens.pp_tokens.length > 0,
            KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, NULL, "Expected file path"));  // TODO Provide source location
    struct kefir_token *token = &directive->pp_tokens.pp_tokens.tokens[0];
    const char *include_path = NULL;
    kefir_bool_t system_include = false;
    if (token->klass == KEFIR_TOKEN_PP_HEADER_NAME) {
        include_path = token->pp_header_name.header_name;
        system_include = token->pp_header_name.system;
    } else if (token->klass == KEFIR_TOKEN_STRING_LITERAL &&
               token->string_literal.type == KEFIR_STRING_LITERAL_TOKEN_MULTIBYTE) {
        include_path = token->string_literal.literal;
    } else {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, NULL, "Expected file path");
    }

    struct kefir_preprocessor_source_file source_file;
    REQUIRE_OK(preprocessor->context->source_locator->open(mem, preprocessor->context->source_locator, include_path,
                                                           system_include, &source_file));

    struct kefir_preprocessor subpreprocessor;
    kefir_result_t res =
        kefir_preprocessor_init(mem, &subpreprocessor, preprocessor->lexer.symbols, &source_file.cursor,
                                preprocessor->lexer.context, preprocessor->context);
    REQUIRE_ELSE(res == KEFIR_OK, {
        source_file.close(mem, &source_file);
        return res;
    });

    res = kefir_preprocessor_run(mem, &subpreprocessor, buffer);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_preprocessor_free(mem, &subpreprocessor);
        source_file.close(mem, &source_file);
        return res;
    });

    res = kefir_preprocessor_free(mem, &subpreprocessor);
    REQUIRE_ELSE(res == KEFIR_OK, {
        source_file.close(mem, &source_file);
        return res;
    });
    REQUIRE_OK(source_file.close(mem, &source_file));
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_run_group(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                            struct kefir_token_buffer *buffer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));

    struct kefir_preprocessor_directive directive;
    kefir_bool_t scan_directives = true;
    enum if_condition_state condition_state = IF_CONDITION_NONE;

    while (scan_directives) {
        struct kefir_lexer_source_cursor_state state;
        REQUIRE_OK(kefir_lexer_source_cursor_save(preprocessor->lexer.cursor, &state));
        REQUIRE_OK(kefir_preprocessor_next_directive(mem, preprocessor, &directive));
        switch (directive.type) {
            case KEFIR_PREPROCESSOR_DIRECTIVE_IFDEF: {
                const struct kefir_preprocessor_macro *macro = NULL;
                kefir_result_t res = kefir_preprocessor_user_macro_scope_at(
                    &preprocessor->context->macros, directive.ifdef_directive.identifier, &macro);
                if (res == KEFIR_NOT_FOUND) {
                    condition_state = IF_CONDITION_FAIL;
                    REQUIRE_OK(kefir_preprocessor_skip_group(mem, preprocessor));
                } else {
                    condition_state = IF_CONDITION_SUCCESS;
                    REQUIRE_OK(res);
                    REQUIRE_OK(kefir_preprocessor_run_group(mem, preprocessor, buffer));
                }
            } break;

            case KEFIR_PREPROCESSOR_DIRECTIVE_IFNDEF: {
                const struct kefir_preprocessor_macro *macro = NULL;
                kefir_result_t res = kefir_preprocessor_user_macro_scope_at(
                    &preprocessor->context->macros, directive.ifdef_directive.identifier, &macro);
                if (res == KEFIR_NOT_FOUND) {
                    condition_state = IF_CONDITION_SUCCESS;
                    REQUIRE_OK(kefir_preprocessor_run_group(mem, preprocessor, buffer));
                } else {
                    REQUIRE_OK(res);
                    condition_state = IF_CONDITION_FAIL;
                    REQUIRE_OK(kefir_preprocessor_skip_group(mem, preprocessor));
                }
            } break;

            case KEFIR_PREPROCESSOR_DIRECTIVE_ENDIF:
                if (condition_state == IF_CONDITION_NONE) {
                    REQUIRE_OK(kefir_lexer_source_cursor_restore(preprocessor->lexer.cursor, &state));
                    scan_directives = false;
                } else {
                    condition_state = IF_CONDITION_NONE;
                }
                break;

            case KEFIR_PREPROCESSOR_DIRECTIVE_ELSE:
                if (condition_state == IF_CONDITION_NONE) {
                    REQUIRE_OK(kefir_lexer_source_cursor_restore(preprocessor->lexer.cursor, &state));
                    scan_directives = false;
                } else if (condition_state == IF_CONDITION_FAIL) {
                    condition_state = IF_CONDITION_SUCCESS;
                    REQUIRE_OK(kefir_preprocessor_run_group(mem, preprocessor, buffer));
                } else {
                    REQUIRE_OK(kefir_preprocessor_skip_group(mem, preprocessor));
                }
                break;

            case KEFIR_PREPROCESSOR_DIRECTIVE_IF:
            case KEFIR_PREPROCESSOR_DIRECTIVE_ELIF:
                return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "If and elif directives are not implemented yet");

            case KEFIR_PREPROCESSOR_DIRECTIVE_INCLUDE:
                REQUIRE_OK(process_include(mem, preprocessor, buffer, &directive));
                break;

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
    }

    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_run(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                      struct kefir_token_buffer *buffer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));

    REQUIRE_OK(kefir_preprocessor_run_group(mem, preprocessor, buffer));
    return KEFIR_OK;
}
