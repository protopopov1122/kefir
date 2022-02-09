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
#include "kefir/parser/parser.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/constant_expression.h"
#include "kefir/preprocessor/format.h"

kefir_result_t kefir_preprocessor_context_init(struct kefir_mem *mem, struct kefir_preprocessor_context *context,
                                               const struct kefir_preprocessor_source_locator *locator,
                                               struct kefir_ast_context *ast_context,
                                               const struct kefir_preprocessor_context_extensions *extensions) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor context"));
    REQUIRE(locator != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor source locator"));
    REQUIRE(ast_context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor AST context"));

    REQUIRE_OK(kefir_preprocessor_user_macro_scope_init(NULL, &context->user_macros));
    context->source_locator = locator;
    context->ast_context = ast_context;

    // Predefined macros
    context->environment.timestamp = time(NULL);
    context->environment.hosted = true;
    context->environment.version = 201112L;

    // Environment macros
    context->environment.stdc_iso10646 = 0;
    context->environment.stdc_mb_might_neq_wc = false;
    context->environment.stdc_utf16 = false;
    context->environment.stdc_utf32 = false;

#ifdef __STDC_ISO_10646__
    context->environment.stdc_iso10646 = __STDC_ISO_10646__;
#endif
#ifdef __STDC_MB_MIGHT_NEQ_WC__
    context->environment.stdc_mb_might_neq_wc = true;
#endif
#ifdef __STDC_UTF_16__
    context->environment.stdc_utf16 = true;
#endif
#ifdef __STDC_UTF_32__
    context->environment.stdc_utf32 = true;
#endif

    // Conditional macros
    context->environment.stdc_analyzable = false;
    context->environment.stdc_iec559 = true;
    context->environment.stdc_iec559_complex = false;
    context->environment.stdc_lib_ext1 = 0;
    context->environment.stdc_no_atomics = true;
    context->environment.stdc_no_complex = true;
    context->environment.stdc_no_threads = false;
    context->environment.stdc_no_vla = false;

    // Extension macros
    context->environment.data_model = NULL;

    context->extensions = extensions;
    context->extensions_payload = NULL;
    kefir_result_t res;
    KEFIR_RUN_EXTENSION0(&res, mem, context, on_init);
    REQUIRE_OK(res);
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_context_free(struct kefir_mem *mem, struct kefir_preprocessor_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor context"));

    kefir_result_t res;
    KEFIR_RUN_EXTENSION0(&res, mem, context, on_free);
    REQUIRE_OK(res);
    context->extensions = NULL;
    context->extensions_payload = NULL;

    REQUIRE_OK(kefir_preprocessor_user_macro_scope_free(mem, &context->user_macros));
    context->source_locator = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_init(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                       struct kefir_symbol_table *symbols, struct kefir_lexer_source_cursor *cursor,
                                       const struct kefir_lexer_context *context,
                                       struct kefir_preprocessor_context *preprocessor_context,
                                       const char *current_filepath,
                                       const struct kefir_preprocessor_extensions *extensions) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer source cursor"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser context"));
    REQUIRE(preprocessor_context != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor context"));

    preprocessor->context = preprocessor_context;
    REQUIRE_OK(kefir_lexer_init(mem, &preprocessor->lexer, symbols, cursor, context,
                                extensions != NULL ? extensions->lexer_extensions : NULL));
    kefir_result_t res =
        kefir_preprocessor_directive_scanner_init(&preprocessor->directive_scanner, &preprocessor->lexer);
    REQUIRE_CHAIN(&res,
                  kefir_preprocessor_predefined_macro_scope_init(mem, &preprocessor->predefined_macros, preprocessor));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_lexer_free(mem, &preprocessor->lexer);
        return res;
    });
    res = kefir_preprocessor_overlay_macro_scope_init(
        &preprocessor->macro_overlay, &preprocessor->predefined_macros.scope, &preprocessor_context->user_macros.scope);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_preprocessor_predefined_macro_scope_free(mem, &preprocessor->predefined_macros);
        kefir_lexer_free(mem, &preprocessor->lexer);
        return res;
    });
    preprocessor->macros = &preprocessor->macro_overlay.scope;
    preprocessor->current_filepath = current_filepath;
    preprocessor->parent = NULL;

    preprocessor->extensions = extensions;
    preprocessor->extension_payload = NULL;
    if (preprocessor->extensions != NULL) {
        KEFIR_RUN_EXTENSION0(&res, mem, preprocessor, on_init);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_preprocessor_predefined_macro_scope_free(mem, &preprocessor->predefined_macros);
            kefir_lexer_free(mem, &preprocessor->lexer);
            return res;
        });
    }
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_free(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));

    if (preprocessor->extensions != NULL) {
        kefir_result_t res;
        KEFIR_RUN_EXTENSION0(&res, mem, preprocessor, on_free);
        REQUIRE_OK(res);
        preprocessor->extensions = NULL;
        preprocessor->extension_payload = NULL;
    }
    REQUIRE_OK(kefir_preprocessor_predefined_macro_scope_free(mem, &preprocessor->predefined_macros));
    REQUIRE_OK(kefir_lexer_free(mem, &preprocessor->lexer));
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_skip_group(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));

    kefir_preprocessor_directive_type_t directive;
    kefir_size_t nested_ifs = 0;
    kefir_bool_t skip = true;

    while (skip) {
        struct kefir_preprocessor_directive_scanner_state scanner_state;
        REQUIRE_OK(kefir_preprocessor_directive_scanner_save(&preprocessor->directive_scanner, &scanner_state));
        REQUIRE_OK(kefir_preprocessor_directive_scanner_match(mem, &preprocessor->directive_scanner, &directive));
        switch (directive) {
            case KEFIR_PREPROCESSOR_DIRECTIVE_IF:
            case KEFIR_PREPROCESSOR_DIRECTIVE_IFDEF:
            case KEFIR_PREPROCESSOR_DIRECTIVE_IFNDEF:
                nested_ifs++;
                REQUIRE_OK(kefir_preprocessor_directive_scanner_skip_line(&preprocessor->directive_scanner));
                break;

            case KEFIR_PREPROCESSOR_DIRECTIVE_ELIF:
            case KEFIR_PREPROCESSOR_DIRECTIVE_ELSE:
                if (nested_ifs == 0) {
                    REQUIRE_OK(
                        kefir_preprocessor_directive_scanner_restore(&preprocessor->directive_scanner, &scanner_state));
                    skip = false;
                }
                break;

            case KEFIR_PREPROCESSOR_DIRECTIVE_ENDIF:
                if (nested_ifs > 0) {
                    nested_ifs--;
                    REQUIRE_OK(kefir_preprocessor_directive_scanner_skip_line(&preprocessor->directive_scanner));
                } else {
                    REQUIRE_OK(
                        kefir_preprocessor_directive_scanner_restore(&preprocessor->directive_scanner, &scanner_state));
                    skip = false;
                }
                break;

            case KEFIR_PREPROCESSOR_DIRECTIVE_SENTINEL:
                skip = false;
                break;

            case KEFIR_PREPROCESSOR_DIRECTIVE_INCLUDE:
            case KEFIR_PREPROCESSOR_DIRECTIVE_DEFINE:
            case KEFIR_PREPROCESSOR_DIRECTIVE_UNDEF:
            case KEFIR_PREPROCESSOR_DIRECTIVE_LINE:
            case KEFIR_PREPROCESSOR_DIRECTIVE_ERROR:
            case KEFIR_PREPROCESSOR_DIRECTIVE_PRAGMA:
            case KEFIR_PREPROCESSOR_DIRECTIVE_EMPTY:
            case KEFIR_PREPROCESSOR_DIRECTIVE_NON:
            case KEFIR_PREPROCESSOR_DIRECTIVE_PP_TOKEN:
                REQUIRE_OK(kefir_preprocessor_directive_scanner_skip_line(&preprocessor->directive_scanner));
                break;
        }
    }
    return KEFIR_OK;
}

enum if_condition_state { IF_CONDITION_NONE, IF_CONDITION_SUCCESS, IF_CONDITION_FAIL };

static kefir_result_t process_include(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                      struct kefir_token_buffer *buffer,
                                      struct kefir_preprocessor_directive *directive) {
    REQUIRE_OK(kefir_preprocessor_run_substitutions(mem, preprocessor, &directive->pp_tokens,
                                                    KEFIR_PREPROCESSOR_SUBSTITUTION_NORMAL));
    REQUIRE(directive->pp_tokens.length > 0,
            KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &directive->source_location, "Expected file path"));
    struct kefir_token *token = &directive->pp_tokens.tokens[0];
    const char *include_path = NULL;
    kefir_bool_t system_include = false;
    if (token->klass == KEFIR_TOKEN_PP_HEADER_NAME) {
        include_path = token->pp_header_name.header_name;
        system_include = token->pp_header_name.system;
    } else if (token->klass == KEFIR_TOKEN_STRING_LITERAL &&
               token->string_literal.type == KEFIR_STRING_LITERAL_TOKEN_MULTIBYTE) {
        include_path = token->string_literal.literal;
    } else {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &directive->source_location, "Expected file path");
    }

    struct kefir_preprocessor_source_file source_file;
    REQUIRE_OK(preprocessor->context->source_locator->open(mem, preprocessor->context->source_locator, include_path,
                                                           system_include, preprocessor->current_filepath,
                                                           &source_file));

    struct kefir_preprocessor subpreprocessor;
    kefir_result_t res = kefir_preprocessor_init(mem, &subpreprocessor, preprocessor->lexer.symbols,
                                                 &source_file.cursor, preprocessor->lexer.context,
                                                 preprocessor->context, source_file.filepath, preprocessor->extensions);
    REQUIRE_ELSE(res == KEFIR_OK, {
        source_file.close(mem, &source_file);
        return res;
    });
    subpreprocessor.parent = preprocessor;

    res = kefir_preprocessor_run(mem, &subpreprocessor, buffer);
    if (buffer->length > 0 && buffer->tokens[buffer->length - 1].klass == KEFIR_TOKEN_SENTINEL) {
        REQUIRE_CHAIN(&res, kefir_token_buffer_pop(mem, buffer));
    }
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

static kefir_result_t process_define(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                     struct kefir_preprocessor_directive *directive) {
    struct kefir_preprocessor_user_macro *macro = NULL;
    if (directive->define_directive.object) {
        macro = kefir_preprocessor_user_macro_new_object(mem, preprocessor->lexer.symbols,
                                                         directive->define_directive.identifier);
        REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate user macro"));
        kefir_result_t res =
            kefir_token_buffer_insert(mem, &macro->replacement, &directive->define_directive.replacement);
        REQUIRE_CHAIN(&res,
                      kefir_preprocessor_user_macro_scope_insert(mem, &preprocessor->context->user_macros, macro));
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_preprocessor_user_macro_free(mem, macro);
            return res;
        });
    } else {
        macro = kefir_preprocessor_user_macro_new_function(mem, preprocessor->lexer.symbols,
                                                           directive->define_directive.identifier);
        REQUIRE(macro != NULL, KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to allocate user macro"));
        kefir_result_t res =
            kefir_token_buffer_insert(mem, &macro->replacement, &directive->define_directive.replacement);
        REQUIRE_CHAIN(&res, kefir_list_move_all(&macro->parameters, &directive->define_directive.parameters));
        if (res == KEFIR_OK) {
            macro->vararg = directive->define_directive.vararg;
        }
        REQUIRE_CHAIN(&res,
                      kefir_preprocessor_user_macro_scope_insert(mem, &preprocessor->context->user_macros, macro));
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_preprocessor_user_macro_free(mem, macro);
            return res;
        });
    }
    return KEFIR_OK;
}

static kefir_result_t process_undef(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                    struct kefir_preprocessor_directive *directive) {
    REQUIRE_OK(kefir_preprocessor_user_macro_scope_remove(mem, &preprocessor->context->user_macros,
                                                          directive->undef_directive.identifier));
    return KEFIR_OK;
}

static kefir_result_t evaluate_pp_tokens(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                         struct kefir_token_buffer *pp_tokens,
                                         struct kefir_source_location *source_location, kefir_bool_t *result) {
    REQUIRE_OK(kefir_preprocessor_run_substitutions(mem, preprocessor, pp_tokens,
                                                    KEFIR_PREPROCESSOR_SUBSTITUTION_IF_CONDITION));
    struct kefir_token_buffer tokens;
    struct kefir_parser_token_cursor cursor;
    struct kefir_parser parser;
    struct kefir_ast_node_base *expression = NULL;
    struct kefir_ast_constant_expression_value expr_value;

    REQUIRE_OK(kefir_token_buffer_init(&tokens));
    kefir_result_t res = kefir_preprocessor_token_convert_buffer(mem, preprocessor, &tokens, pp_tokens);
    REQUIRE_CHAIN(&res, kefir_parser_token_cursor_init(&cursor, tokens.tokens, tokens.length));
    REQUIRE_CHAIN(&res, kefir_parser_init(mem, &parser, preprocessor->lexer.symbols, &cursor,
                                          preprocessor->extensions != NULL ? preprocessor->extensions->parser : NULL));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &tokens);
        return res;
    });

    res = KEFIR_PARSER_NEXT_EXPRESSION(mem, &parser, &expression);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_parser_free(mem, &parser);
        kefir_token_buffer_free(mem, &tokens);
        return res;
    });

    res = kefir_parser_free(mem, &parser);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &tokens);
        return res;
    });

    res = kefir_token_buffer_free(mem, &tokens);
    REQUIRE_CHAIN(&res, kefir_ast_analyze_node(mem, preprocessor->context->ast_context, expression));
    REQUIRE_CHAIN(&res, kefir_ast_constant_expression_value_evaluate(mem, preprocessor->context->ast_context,
                                                                     expression, &expr_value));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, expression);
        return res;
    });
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, expression));
    switch (expr_value.klass) {
        case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER:
            *result = expr_value.integer != 0;
            break;

        case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT:
            *result = expr_value.floating_point != 0;
            break;

        default:
            return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, source_location, "Unexpected constant expression type");
    }
    return KEFIR_OK;
}

static kefir_result_t process_if(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                 struct kefir_preprocessor_directive *directive,
                                 enum if_condition_state *condition_state, struct kefir_token_buffer *group_buffer) {
    kefir_bool_t condition;
    REQUIRE_OK(evaluate_pp_tokens(mem, preprocessor, &directive->pp_tokens, &directive->source_location, &condition));
    if (!condition) {
        *condition_state = IF_CONDITION_FAIL;
        REQUIRE_OK(kefir_preprocessor_skip_group(mem, preprocessor));
    } else {
        *condition_state = IF_CONDITION_SUCCESS;
        REQUIRE_OK(kefir_preprocessor_run_group(mem, preprocessor, group_buffer));
    }
    return KEFIR_OK;
}

static kefir_result_t process_elif(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                   struct kefir_preprocessor_directive *directive,
                                   enum if_condition_state *condition_state, kefir_bool_t *continue_scan,
                                   struct kefir_token_buffer *group_buffer,
                                   struct kefir_preprocessor_directive_scanner_state *scanner_state) {

    if (*condition_state == IF_CONDITION_NONE) {
        REQUIRE_OK(kefir_preprocessor_directive_scanner_restore(&preprocessor->directive_scanner, scanner_state));
        *continue_scan = false;
    } else if (*condition_state == IF_CONDITION_FAIL) {
        kefir_bool_t condition;
        REQUIRE_OK(
            evaluate_pp_tokens(mem, preprocessor, &directive->pp_tokens, &directive->source_location, &condition));
        if (condition) {
            *condition_state = IF_CONDITION_SUCCESS;
            REQUIRE_OK(kefir_preprocessor_run_group(mem, preprocessor, group_buffer));
        } else {
            REQUIRE_OK(kefir_preprocessor_skip_group(mem, preprocessor));
        }
    } else {
        REQUIRE_OK(kefir_preprocessor_skip_group(mem, preprocessor));
    }
    return KEFIR_OK;
}

static kefir_result_t process_error(struct kefir_mem *mem, struct kefir_preprocessor_directive *directive) {
    char *error_message;
    kefir_size_t error_length;
    REQUIRE_OK(kefir_preprocessor_format_string(mem, &error_message, &error_length, &directive->pp_tokens,
                                                KEFIR_PREPROCESSOR_WHITESPACE_FORMAT_SINGLE_SPACE));
    kefir_result_t res =
        KEFIR_SET_SOURCE_ERRORF(KEFIR_PREPROCESSOR_ERROR_DIRECTIVE, &directive->source_location, "%s", error_message);
    KEFIR_FREE(mem, error_message);
    return res;
}

static kefir_result_t process_line(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                   struct kefir_preprocessor_directive *directive) {
    REQUIRE_OK(kefir_preprocessor_run_substitutions(mem, preprocessor, &directive->pp_tokens,
                                                    KEFIR_PREPROCESSOR_SUBSTITUTION_NORMAL));
    REQUIRE(directive->pp_tokens.length > 0,
            KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &directive->source_location, "Expected line number"));
    struct kefir_token *token = &directive->pp_tokens.tokens[0];
    REQUIRE(token->klass == KEFIR_TOKEN_PP_NUMBER,
            KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &token->source_location, "Expected line number"));

    char *linenum_end;
    unsigned long line = strtoul(token->pp_number.number_literal, &linenum_end, 10);
    REQUIRE(linenum_end == token->pp_number.number_literal + strlen(token->pp_number.number_literal),
            KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &token->source_location,
                                   "Unable to parse line number as unsigned integer"));

    const char *source_file = NULL;
    for (kefir_size_t i = 1; i < directive->pp_tokens.length && source_file == NULL; i++) {
        token = &directive->pp_tokens.tokens[i];
        if (token->klass != KEFIR_TOKEN_PP_WHITESPACE) {
            REQUIRE(token->klass == KEFIR_TOKEN_STRING_LITERAL &&
                        token->string_literal.type == KEFIR_STRING_LITERAL_TOKEN_MULTIBYTE,
                    KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &token->source_location, "Expected valid file name"));
            source_file =
                kefir_symbol_table_insert(mem, preprocessor->lexer.symbols, token->string_literal.literal, NULL);
            REQUIRE(source_file != NULL,
                    KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert file name into symbol table"));
        }
    }

    preprocessor->lexer.cursor->location.line = line;
    if (source_file != NULL) {
        preprocessor->lexer.cursor->location.source = source_file;
    }
    return KEFIR_OK;
}

static kefir_result_t flush_group_buffer(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                         struct kefir_token_buffer *buffer, struct kefir_token_buffer *group_buffer) {
    REQUIRE_OK(
        kefir_preprocessor_run_substitutions(mem, preprocessor, group_buffer, KEFIR_PREPROCESSOR_SUBSTITUTION_NORMAL));
    REQUIRE_OK(kefir_token_buffer_insert(mem, buffer, group_buffer));
    return KEFIR_OK;
}

static kefir_result_t run_directive(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                    struct kefir_token_buffer *buffer, struct kefir_token_buffer *group_buffer,
                                    struct kefir_preprocessor_directive *directive, kefir_bool_t *scan_directives,
                                    enum if_condition_state *condition_state,
                                    struct kefir_preprocessor_directive_scanner_state *scanner_state) {
    switch (directive->type) {
        case KEFIR_PREPROCESSOR_DIRECTIVE_IFDEF: {
            const struct kefir_preprocessor_macro *macro = NULL;
            kefir_result_t res =
                preprocessor->macros->locate(preprocessor->macros, directive->ifdef_directive.identifier, &macro);
            if (res == KEFIR_NOT_FOUND) {
                *condition_state = IF_CONDITION_FAIL;
                REQUIRE_OK(kefir_preprocessor_skip_group(mem, preprocessor));
            } else {
                *condition_state = IF_CONDITION_SUCCESS;
                REQUIRE_OK(res);
                REQUIRE_OK(flush_group_buffer(mem, preprocessor, buffer, group_buffer));
                REQUIRE_OK(kefir_preprocessor_run_group(mem, preprocessor, buffer));
            }
        } break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_IFNDEF: {
            const struct kefir_preprocessor_macro *macro = NULL;
            kefir_result_t res =
                preprocessor->macros->locate(preprocessor->macros, directive->ifdef_directive.identifier, &macro);
            if (res == KEFIR_NOT_FOUND) {
                *condition_state = IF_CONDITION_SUCCESS;
                REQUIRE_OK(flush_group_buffer(mem, preprocessor, buffer, group_buffer));
                REQUIRE_OK(kefir_preprocessor_run_group(mem, preprocessor, buffer));
            } else {
                REQUIRE_OK(res);
                *condition_state = IF_CONDITION_FAIL;
                REQUIRE_OK(kefir_preprocessor_skip_group(mem, preprocessor));
            }
        } break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_ENDIF:
            if (*condition_state == IF_CONDITION_NONE) {
                REQUIRE_OK(
                    kefir_preprocessor_directive_scanner_restore(&preprocessor->directive_scanner, scanner_state));
                *scan_directives = false;
            } else {
                *condition_state = IF_CONDITION_NONE;
            }
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_ELSE:
            if (*condition_state == IF_CONDITION_NONE) {
                REQUIRE_OK(
                    kefir_preprocessor_directive_scanner_restore(&preprocessor->directive_scanner, scanner_state));
                *scan_directives = false;
            } else if (*condition_state == IF_CONDITION_FAIL) {
                *condition_state = IF_CONDITION_SUCCESS;
                REQUIRE_OK(flush_group_buffer(mem, preprocessor, buffer, group_buffer));
                REQUIRE_OK(kefir_preprocessor_run_group(mem, preprocessor, buffer));
            } else {
                REQUIRE_OK(kefir_preprocessor_skip_group(mem, preprocessor));
            }
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_IF:
            REQUIRE_OK(flush_group_buffer(mem, preprocessor, buffer, group_buffer));
            REQUIRE_OK(process_if(mem, preprocessor, directive, condition_state, buffer));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_ELIF:
            REQUIRE_OK(flush_group_buffer(mem, preprocessor, buffer, group_buffer));
            REQUIRE_OK(
                process_elif(mem, preprocessor, directive, condition_state, scan_directives, buffer, scanner_state));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_INCLUDE:
            REQUIRE_OK(flush_group_buffer(mem, preprocessor, buffer, group_buffer));
            REQUIRE_OK(process_include(mem, preprocessor, buffer, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_DEFINE:
            REQUIRE_OK(flush_group_buffer(mem, preprocessor, buffer, group_buffer));
            REQUIRE_OK(process_define(mem, preprocessor, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_UNDEF:
            REQUIRE_OK(flush_group_buffer(mem, preprocessor, buffer, group_buffer));
            REQUIRE_OK(process_undef(mem, preprocessor, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_ERROR:
            REQUIRE_OK(process_error(mem, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_PRAGMA:
            // TODO Implement STDC pragmas
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_LINE:
            REQUIRE_OK(process_line(mem, preprocessor, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_EMPTY:
        case KEFIR_PREPROCESSOR_DIRECTIVE_NON:
            // Skip empty and unknown directives
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_PP_TOKEN:
            REQUIRE_OK(kefir_token_buffer_emplace(mem, group_buffer, &directive->pp_token));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_SENTINEL:
            *scan_directives = false;
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t kefir_preprocessor_run_group_impl(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                                        struct kefir_token_buffer *buffer,
                                                        struct kefir_token_buffer *group_buffer) {
    struct kefir_preprocessor_directive directive;
    kefir_bool_t scan_directives = true;
    enum if_condition_state condition_state = IF_CONDITION_NONE;

    while (scan_directives) {
        struct kefir_preprocessor_directive_scanner_state scanner_state;
        REQUIRE_OK(kefir_preprocessor_directive_scanner_save(&preprocessor->directive_scanner, &scanner_state));
        REQUIRE_OK(kefir_preprocessor_directive_scanner_next(mem, &preprocessor->directive_scanner, &directive));
        kefir_result_t res;
        KEFIR_RUN_EXTENSION(&res, mem, preprocessor, on_next_directive, &directive);
        REQUIRE_CHAIN(&res, run_directive(mem, preprocessor, buffer, group_buffer, &directive, &scan_directives,
                                          &condition_state, &scanner_state));
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_preprocessor_directive_free(mem, &directive);
            return res;
        });
        REQUIRE_OK(kefir_preprocessor_directive_free(mem, &directive));
    }
    REQUIRE_OK(flush_group_buffer(mem, preprocessor, buffer, group_buffer));
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_run_group(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                            struct kefir_token_buffer *buffer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));

    struct kefir_token_buffer group_buffer;
    REQUIRE_OK(kefir_token_buffer_init(&group_buffer));
    kefir_result_t res = kefir_preprocessor_run_group_impl(mem, preprocessor, buffer, &group_buffer);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &group_buffer);
        return res;
    });
    REQUIRE_OK(kefir_token_buffer_free(mem, &group_buffer));
    return KEFIR_OK;
}

static kefir_result_t insert_sentinel(struct kefir_mem *mem, struct kefir_preprocessor_directive *directive,
                                      struct kefir_token_buffer *buffer) {
    struct kefir_token token;
    REQUIRE_OK(kefir_token_new_sentinel(&token));
    token.source_location = directive->source_location;
    REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &token));
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_run(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                      struct kefir_token_buffer *buffer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));
    REQUIRE(buffer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid token buffer"));

    kefir_result_t res;
    KEFIR_RUN_EXTENSION(&res, mem, preprocessor, before_run, buffer);
    REQUIRE_OK(res);

    REQUIRE_OK(kefir_preprocessor_run_group(mem, preprocessor, buffer));

    struct kefir_preprocessor_directive directive;
    REQUIRE_OK(kefir_preprocessor_directive_scanner_next(mem, &preprocessor->directive_scanner, &directive));
    REQUIRE_ELSE(directive.type == KEFIR_PREPROCESSOR_DIRECTIVE_SENTINEL, {
        kefir_preprocessor_directive_free(mem, &directive);
        return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &directive.source_location,
                                      "Unexpected preprocessor directive/token");
    });
    res = insert_sentinel(mem, &directive, buffer);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_preprocessor_directive_free(mem, &directive);
        return res;
    });
    REQUIRE_OK(kefir_preprocessor_directive_free(mem, &directive));

    KEFIR_RUN_EXTENSION(&res, mem, preprocessor, after_run, buffer);
    REQUIRE_OK(res);
    return KEFIR_OK;
}
