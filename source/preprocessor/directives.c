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
#include "kefir/core/source_error.h"
#include "kefir/util/char32.h"
#include <string.h>

kefir_result_t kefir_preprocessor_directive_scanner_init(struct kefir_preprocessor_directive_scanner *directive_scanner,
                                                         struct kefir_lexer *lexer) {
    REQUIRE(directive_scanner != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor directive lexer"));
    REQUIRE(lexer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid lexer"));

    directive_scanner->lexer = lexer;
    directive_scanner->newline_flag = true;
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_directive_scanner_save(
    const struct kefir_preprocessor_directive_scanner *directive_scanner,
    struct kefir_preprocessor_directive_scanner_state *state) {
    REQUIRE(directive_scanner != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor directive scanner"));
    REQUIRE(state != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor directive lexer state"));

    REQUIRE_OK(kefir_lexer_source_cursor_save(directive_scanner->lexer->cursor, &state->cursor_state));
    state->newline_flag = directive_scanner->newline_flag;
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_directive_scanner_restore(
    struct kefir_preprocessor_directive_scanner *directive_scanner,
    const struct kefir_preprocessor_directive_scanner_state *state) {
    REQUIRE(directive_scanner != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor directive scanner"));
    REQUIRE(state != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor directive lexer state"));

    REQUIRE_OK(kefir_lexer_source_cursor_restore(directive_scanner->lexer->cursor, &state->cursor_state));
    directive_scanner->newline_flag = state->newline_flag;
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_directive_scanner_skip_line(
    struct kefir_preprocessor_directive_scanner *directive_scanner) {
    kefir_bool_t skip = true;
    while (skip) {
        kefir_char32_t chr = kefir_lexer_source_cursor_at(directive_scanner->lexer->cursor, 0);
        if (chr == U'\0') {
            skip = false;
        } else {
            skip = chr != directive_scanner->lexer->context->newline;
            REQUIRE_OK(kefir_lexer_source_cursor_next(directive_scanner->lexer->cursor, 1));
        }
    }
    directive_scanner->newline_flag = true;
    return KEFIR_OK;
}

static kefir_result_t skip_whitespaces(struct kefir_mem *mem,
                                       struct kefir_preprocessor_directive_scanner *directive_scanner,
                                       struct kefir_token *token) {
    kefir_bool_t skip_whitespaces = true;
    while (skip_whitespaces) {
        REQUIRE_OK(kefir_preprocessor_tokenize_next(mem, directive_scanner->lexer, token));
        if (token->klass != KEFIR_TOKEN_PP_WHITESPACE || token->pp_whitespace.newline) {
            skip_whitespaces = false;
        } else {
            REQUIRE_OK(kefir_token_free(mem, token));
        }
    }
    return KEFIR_OK;
}

static kefir_result_t scan_pp_tokens(struct kefir_mem *mem,
                                     struct kefir_preprocessor_directive_scanner *directive_scanner,
                                     struct kefir_token_buffer *buffer) {
    struct kefir_token next_token;
    kefir_result_t scan_tokens = true;
    while (scan_tokens) {
        REQUIRE_OK(kefir_preprocessor_tokenize_next(mem, directive_scanner->lexer, &next_token));

        if (next_token.klass == KEFIR_TOKEN_SENTINEL ||
            (next_token.klass == KEFIR_TOKEN_PP_WHITESPACE && next_token.pp_whitespace.newline)) {
            scan_tokens = false;
            REQUIRE_OK(kefir_token_free(mem, &next_token));
        } else {
            REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &next_token));
        }
    }
    directive_scanner->newline_flag = true;
    return KEFIR_OK;
}

static kefir_result_t skip_whitespaces_until(struct kefir_lexer_source_cursor *cursor, kefir_char32_t terminator) {
    kefir_char32_t chr = kefir_lexer_source_cursor_at(cursor, 0);
    while (kefir_isspace32(chr) && chr != terminator) {
        REQUIRE_OK(kefir_lexer_source_cursor_next(cursor, 1));
        chr = kefir_lexer_source_cursor_at(cursor, 0);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_directive_scanner_match(
    struct kefir_mem *mem, struct kefir_preprocessor_directive_scanner *directive_scanner,
    kefir_preprocessor_directive_type_t *directive_type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(directive_scanner != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor directive lexer"));
    REQUIRE(directive_type != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to directive type"));

    struct kefir_lexer_source_cursor_state initial_state, hash_state;
    REQUIRE_OK(kefir_lexer_source_cursor_save(directive_scanner->lexer->cursor, &initial_state));

    REQUIRE_OK(skip_whitespaces_until(directive_scanner->lexer->cursor, directive_scanner->lexer->context->newline));
    kefir_char32_t chr = kefir_lexer_source_cursor_at(directive_scanner->lexer->cursor, 0);
    if (chr == U'\0') {
        *directive_type = KEFIR_PREPROCESSOR_DIRECTIVE_SENTINEL;
        directive_scanner->newline_flag = true;
        return KEFIR_OK;
    }
    if (chr != U'#') {
        REQUIRE_OK(kefir_lexer_source_cursor_restore(directive_scanner->lexer->cursor, &initial_state));
        *directive_type = KEFIR_PREPROCESSOR_DIRECTIVE_PP_TOKEN;
        directive_scanner->newline_flag = true;
        return KEFIR_OK;
    }
    REQUIRE_OK(kefir_lexer_source_cursor_save(directive_scanner->lexer->cursor, &hash_state));
    REQUIRE_OK(kefir_lexer_source_cursor_next(directive_scanner->lexer->cursor, 1));
    REQUIRE_OK(skip_whitespaces_until(directive_scanner->lexer->cursor, directive_scanner->lexer->context->newline));
    chr = kefir_lexer_source_cursor_at(directive_scanner->lexer->cursor, 0);
    if (chr == directive_scanner->lexer->context->newline) {
        *directive_type = KEFIR_PREPROCESSOR_DIRECTIVE_EMPTY;
        directive_scanner->newline_flag = true;
        return KEFIR_OK;
    }

#define MAX_DIRECTIVE_NAME 64
    kefir_char32_t directive_name[MAX_DIRECTIVE_NAME] = {0};
    kefir_size_t directive_name_idx = 0;
    while (kefir_isnondigit32(chr) && directive_name_idx + 1 < MAX_DIRECTIVE_NAME) {
        directive_name[directive_name_idx++] = chr;
        REQUIRE_OK(kefir_lexer_source_cursor_next(directive_scanner->lexer->cursor, 1));
        chr = kefir_lexer_source_cursor_at(directive_scanner->lexer->cursor, 0);
    }
#undef MAX_DIRECTIVE_NAME

    static const struct {
        const kefir_char32_t *literal;
        kefir_preprocessor_directive_type_t directive;
    } KnownDirectives[] = {
        {U"if", KEFIR_PREPROCESSOR_DIRECTIVE_IF},           {U"ifdef", KEFIR_PREPROCESSOR_DIRECTIVE_IFDEF},
        {U"ifndef", KEFIR_PREPROCESSOR_DIRECTIVE_IFNDEF},   {U"elif", KEFIR_PREPROCESSOR_DIRECTIVE_ELIF},
        {U"else", KEFIR_PREPROCESSOR_DIRECTIVE_ELSE},       {U"endif", KEFIR_PREPROCESSOR_DIRECTIVE_ENDIF},
        {U"include", KEFIR_PREPROCESSOR_DIRECTIVE_INCLUDE}, {U"define", KEFIR_PREPROCESSOR_DIRECTIVE_DEFINE},
        {U"undef", KEFIR_PREPROCESSOR_DIRECTIVE_UNDEF},     {U"error", KEFIR_PREPROCESSOR_DIRECTIVE_ERROR}};
    for (kefir_size_t i = 0; i < sizeof(KnownDirectives) / sizeof(KnownDirectives[0]); i++) {
        if (kefir_strcmp32(KnownDirectives[i].literal, directive_name) == 0) {
            *directive_type = KnownDirectives[i].directive;
            directive_scanner->newline_flag = false;
            return KEFIR_OK;
        }
    }

    REQUIRE_OK(kefir_lexer_source_cursor_restore(directive_scanner->lexer->cursor, &hash_state));
    *directive_type = KEFIR_PREPROCESSOR_DIRECTIVE_NON;
    directive_scanner->newline_flag = false;
    return KEFIR_OK;
}

static kefir_result_t next_if(struct kefir_mem *mem, struct kefir_preprocessor_directive_scanner *directive_scanner,
                              struct kefir_preprocessor_directive *directive) {
    directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_IF;
    REQUIRE_OK(kefir_token_buffer_init(&directive->if_directive.condition));

    kefir_result_t res = scan_pp_tokens(mem, directive_scanner, &directive->if_directive.condition);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &directive->if_directive.condition);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t next_elif(struct kefir_mem *mem, struct kefir_preprocessor_directive_scanner *directive_scanner,
                                struct kefir_preprocessor_directive *directive) {
    directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_ELIF;
    REQUIRE_OK(kefir_token_buffer_init(&directive->if_directive.condition));

    kefir_result_t res = scan_pp_tokens(mem, directive_scanner, &directive->if_directive.condition);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &directive->if_directive.condition);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t next_ifdef(struct kefir_mem *mem, struct kefir_preprocessor_directive_scanner *directive_scanner,
                                 struct kefir_preprocessor_directive *directive, kefir_bool_t inverse) {
    struct kefir_token token;
    REQUIRE_OK(skip_whitespaces(mem, directive_scanner, &token));
    REQUIRE_ELSE(token.klass == KEFIR_TOKEN_IDENTIFIER, {
        kefir_token_free(mem, &token);
        return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &token.source_location, "Expected identifier");
    });
    const char *identifier = kefir_symbol_table_insert(mem, directive_scanner->lexer->symbols, token.identifier, NULL);
    REQUIRE_ELSE(identifier != NULL, {
        kefir_token_free(mem, &token);
        return KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert identifier into symbol table");
    });
    REQUIRE_OK(kefir_token_free(mem, &token));
    REQUIRE_OK(kefir_preprocessor_directive_scanner_skip_line(directive_scanner));

    directive->type = inverse ? KEFIR_PREPROCESSOR_DIRECTIVE_IFNDEF : KEFIR_PREPROCESSOR_DIRECTIVE_IFDEF;
    directive->ifdef_directive.identifier = identifier;
    return KEFIR_OK;
}

static kefir_result_t next_else(struct kefir_preprocessor_directive_scanner *directive_scanner,
                                struct kefir_preprocessor_directive *directive) {
    directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_ELSE;
    REQUIRE_OK(kefir_preprocessor_directive_scanner_skip_line(directive_scanner));
    return KEFIR_OK;
}

static kefir_result_t next_endif(struct kefir_preprocessor_directive_scanner *directive_scanner,
                                 struct kefir_preprocessor_directive *directive) {
    directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_ENDIF;
    REQUIRE_OK(kefir_preprocessor_directive_scanner_skip_line(directive_scanner));
    return KEFIR_OK;
}

static kefir_result_t next_include(struct kefir_mem *mem,
                                   struct kefir_preprocessor_directive_scanner *directive_scanner,
                                   struct kefir_preprocessor_directive *directive) {
    directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_INCLUDE;
    REQUIRE_OK(kefir_token_buffer_init(&directive->pp_tokens));

    struct kefir_lexer_source_cursor_state state;
    struct kefir_token token;
    kefir_result_t res = kefir_lexer_source_cursor_save(directive_scanner->lexer->cursor, &state);
    REQUIRE_CHAIN(&res,
                  skip_whitespaces_until(directive_scanner->lexer->cursor, directive_scanner->lexer->context->newline));
    REQUIRE_CHAIN(&res, kefir_lexer_match_pp_header_name(mem, directive_scanner->lexer, &token));
    if (res == KEFIR_NO_MATCH) {
        res = kefir_lexer_source_cursor_restore(directive_scanner->lexer->cursor, &state);
        REQUIRE_CHAIN(
            &res, skip_whitespaces_until(directive_scanner->lexer->cursor, directive_scanner->lexer->context->newline));
        REQUIRE_CHAIN(&res, scan_pp_tokens(mem, directive_scanner, &directive->pp_tokens));
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_buffer_free(mem, &directive->pp_tokens);
            return res;
        });
    } else {
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_buffer_free(mem, &directive->pp_tokens);
            return res;
        });
        res = kefir_token_buffer_emplace(mem, &directive->pp_tokens, &token);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_free(mem, &token);
            kefir_token_buffer_free(mem, &directive->pp_tokens);
            return res;
        });
        res = kefir_preprocessor_directive_scanner_skip_line(directive_scanner);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_buffer_free(mem, &directive->pp_tokens);
            return res;
        });
    }
    return KEFIR_OK;
}

static kefir_result_t next_define_replacement_list(struct kefir_mem *mem,
                                                   struct kefir_preprocessor_directive_scanner *directive_scanner,
                                                   struct kefir_preprocessor_directive *directive,
                                                   struct kefir_token *token) {
    if (token->klass == KEFIR_TOKEN_PP_WHITESPACE && !token->pp_whitespace.newline) {
        REQUIRE_OK(skip_whitespaces(mem, directive_scanner, token));
    }
    REQUIRE_OK(kefir_token_buffer_init(&directive->define_directive.replacement));
    kefir_result_t res;
    while (token->klass != KEFIR_TOKEN_SENTINEL &&
           (token->klass != KEFIR_TOKEN_PP_WHITESPACE || !token->pp_whitespace.newline)) {
        res = kefir_token_buffer_emplace(mem, &directive->define_directive.replacement, token);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_free(mem, token);
            kefir_token_buffer_free(mem, &directive->define_directive.replacement);
            return res;
        });

        res = kefir_preprocessor_tokenize_next(mem, directive_scanner->lexer, token);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_buffer_free(mem, &directive->define_directive.replacement);
            return res;
        });
    }
    res = kefir_token_free(mem, token);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &directive->define_directive.replacement);
        return res;
    });

    kefir_bool_t drop_ws = true;
    while (directive->define_directive.replacement.length > 0 && drop_ws) {
        const struct kefir_token *last_token =
            &directive->define_directive.replacement.tokens[directive->define_directive.replacement.length - 1];
        drop_ws = last_token->klass == KEFIR_TOKEN_PP_WHITESPACE;
        if (drop_ws) {
            kefir_result_t res = kefir_token_buffer_pop(mem, &directive->define_directive.replacement);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_token_buffer_free(mem, &directive->define_directive.replacement);
                return res;
            });
        }
    }
    directive_scanner->newline_flag = true;
    return KEFIR_OK;
}

static kefir_result_t scan_define_parameters(struct kefir_mem *mem,
                                             struct kefir_preprocessor_directive_scanner *directive_scanner,
                                             struct kefir_preprocessor_directive *directive,
                                             struct kefir_token *token) {
    kefir_bool_t scan_params = true;
    directive->define_directive.vararg = false;
    while (scan_params) {
        REQUIRE_OK(skip_whitespaces(mem, directive_scanner, token));
        if (token->klass == KEFIR_TOKEN_IDENTIFIER) {
            const char *identifier =
                kefir_symbol_table_insert(mem, directive_scanner->lexer->symbols, token->identifier, NULL);
            REQUIRE(identifier != NULL,
                    KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate macro argument identifier"));
            REQUIRE_OK(kefir_list_insert_after(mem, &directive->define_directive.parameters,
                                               kefir_list_tail(&directive->define_directive.parameters),
                                               (void *) identifier));
            REQUIRE_OK(skip_whitespaces(mem, directive_scanner, token));
            if (token->klass == KEFIR_TOKEN_PUNCTUATOR && token->punctuator == KEFIR_PUNCTUATOR_RIGHT_PARENTHESE) {
                scan_params = false;
            } else {
                REQUIRE(token->klass == KEFIR_TOKEN_PUNCTUATOR && token->punctuator == KEFIR_PUNCTUATOR_COMMA,
                        KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &token->source_location,
                                               "Expected either comma or right parenthese"));
            }
        } else if (token->klass == KEFIR_TOKEN_PUNCTUATOR && token->punctuator == KEFIR_PUNCTUATOR_ELLIPSIS) {
            directive->define_directive.vararg = true;
            scan_params = false;
        } else {
            return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &token->source_location,
                                          "Expected either parameter or ellipsis");
        }
    }
    if (token->klass != KEFIR_TOKEN_PUNCTUATOR || token->punctuator != KEFIR_PUNCTUATOR_RIGHT_PARENTHESE) {
        REQUIRE_OK(skip_whitespaces(mem, directive_scanner, token));
    }
    REQUIRE(token->klass == KEFIR_TOKEN_PUNCTUATOR && token->punctuator == KEFIR_PUNCTUATOR_RIGHT_PARENTHESE,
            KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &token->source_location, "Expected right parenthese"));
    REQUIRE_OK(skip_whitespaces(mem, directive_scanner, token));
    return KEFIR_OK;
}

static kefir_result_t next_define_function(struct kefir_mem *mem,
                                           struct kefir_preprocessor_directive_scanner *directive_scanner,
                                           struct kefir_preprocessor_directive *directive, struct kefir_token *token) {
    REQUIRE_OK(kefir_list_init(&directive->define_directive.parameters));
    kefir_result_t res = scan_define_parameters(mem, directive_scanner, directive, token);
    REQUIRE_CHAIN(&res, next_define_replacement_list(mem, directive_scanner, directive, token));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &directive->define_directive.parameters);
        return res;
    });
    directive->define_directive.object = false;
    return KEFIR_OK;
}

static kefir_result_t next_define(struct kefir_mem *mem, struct kefir_preprocessor_directive_scanner *directive_scanner,
                                  struct kefir_preprocessor_directive *directive) {
    directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_DEFINE;

    struct kefir_token token;
    REQUIRE_OK(skip_whitespaces(mem, directive_scanner, &token));
    REQUIRE(token.klass == KEFIR_TOKEN_IDENTIFIER,
            KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &token.source_location, "Expected identifier"));
    directive->define_directive.identifier =
        kefir_symbol_table_insert(mem, directive_scanner->lexer->symbols, token.identifier, NULL);
    REQUIRE(directive->define_directive.identifier != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate identifier"));

    REQUIRE_OK(kefir_preprocessor_tokenize_next(mem, directive_scanner->lexer, &token));
    if (token.klass == KEFIR_TOKEN_PUNCTUATOR && token.punctuator == KEFIR_PUNCTUATOR_LEFT_PARENTHESE) {
        REQUIRE_OK(next_define_function(mem, directive_scanner, directive, &token));
    } else {
        directive->define_directive.object = true;
        REQUIRE_OK(next_define_replacement_list(mem, directive_scanner, directive, &token));
    }
    return KEFIR_OK;
}

static kefir_result_t next_undef(struct kefir_mem *mem, struct kefir_preprocessor_directive_scanner *directive_scanner,
                                 struct kefir_preprocessor_directive *directive) {
    struct kefir_token token;
    REQUIRE_OK(skip_whitespaces(mem, directive_scanner, &token));
    REQUIRE_ELSE(token.klass == KEFIR_TOKEN_IDENTIFIER, {
        kefir_token_free(mem, &token);
        return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &token.source_location, "Expected identifier");
    });
    const char *identifier = kefir_symbol_table_insert(mem, directive_scanner->lexer->symbols, token.identifier, NULL);
    REQUIRE_ELSE(identifier != NULL, {
        kefir_token_free(mem, &token);
        return KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert identifier into symbol table");
    });
    REQUIRE_OK(kefir_token_free(mem, &token));
    REQUIRE_OK(kefir_preprocessor_directive_scanner_skip_line(directive_scanner));

    directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_UNDEF;
    directive->undef_directive.identifier = identifier;
    return KEFIR_OK;
}

static kefir_result_t next_error(struct kefir_mem *mem, struct kefir_preprocessor_directive_scanner *directive_scanner,
                                 struct kefir_preprocessor_directive *directive) {
    directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_ERROR;
    REQUIRE_OK(kefir_token_buffer_init(&directive->pp_tokens));

    kefir_result_t res = scan_pp_tokens(mem, directive_scanner, &directive->pp_tokens);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &directive->pp_tokens);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t next_non_directive(struct kefir_mem *mem,
                                         struct kefir_preprocessor_directive_scanner *directive_scanner,
                                         struct kefir_preprocessor_directive *directive) {
    directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_NON;
    REQUIRE_OK(kefir_token_buffer_init(&directive->pp_tokens));

    kefir_result_t res = scan_pp_tokens(mem, directive_scanner, &directive->pp_tokens);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &directive->pp_tokens);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t next_pp_token(struct kefir_mem *mem,
                                    struct kefir_preprocessor_directive_scanner *directive_scanner,
                                    struct kefir_preprocessor_directive *directive) {
    directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_PP_TOKEN;
    REQUIRE_OK(kefir_preprocessor_tokenize_next(mem, directive_scanner->lexer, &directive->pp_token));
    if (directive->pp_token.klass == KEFIR_TOKEN_SENTINEL) {
        REQUIRE_OK(kefir_token_free(mem, &directive->pp_token));
        directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_SENTINEL;
        directive_scanner->newline_flag = true;
    } else {
        directive_scanner->newline_flag =
            directive->pp_token.klass == KEFIR_TOKEN_PP_WHITESPACE && directive->pp_token.pp_whitespace.newline;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_directive_scanner_next(struct kefir_mem *mem,
                                                         struct kefir_preprocessor_directive_scanner *directive_scanner,
                                                         struct kefir_preprocessor_directive *directive) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(directive_scanner != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor directive lexer"));
    REQUIRE(directive != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor directive"));

    struct kefir_source_location location = directive_scanner->lexer->cursor->location;
    if (!directive_scanner->newline_flag) {
        REQUIRE_OK(next_pp_token(mem, directive_scanner, directive));
        directive->source_location = location;
        return KEFIR_OK;
    }

    kefir_preprocessor_directive_type_t directive_type;
    REQUIRE_OK(kefir_preprocessor_directive_scanner_match(mem, directive_scanner, &directive_type));
    switch (directive_type) {
        case KEFIR_PREPROCESSOR_DIRECTIVE_IF:
            REQUIRE_OK(next_if(mem, directive_scanner, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_IFDEF:
            REQUIRE_OK(next_ifdef(mem, directive_scanner, directive, false));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_IFNDEF:
            REQUIRE_OK(next_ifdef(mem, directive_scanner, directive, true));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_ELIF:
            REQUIRE_OK(next_elif(mem, directive_scanner, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_ELSE:
            REQUIRE_OK(next_else(directive_scanner, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_ENDIF:
            REQUIRE_OK(next_endif(directive_scanner, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_INCLUDE:
            REQUIRE_OK(next_include(mem, directive_scanner, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_DEFINE:
            REQUIRE_OK(next_define(mem, directive_scanner, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_UNDEF:
            REQUIRE_OK(next_undef(mem, directive_scanner, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_ERROR:
            REQUIRE_OK(next_error(mem, directive_scanner, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_LINE:
        case KEFIR_PREPROCESSOR_DIRECTIVE_PRAGMA:
            return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Preprocessor directive is not implemented yet");

        case KEFIR_PREPROCESSOR_DIRECTIVE_EMPTY:
            directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_EMPTY;
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_NON:
            REQUIRE_OK(next_non_directive(mem, directive_scanner, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_PP_TOKEN:
            REQUIRE_OK(next_pp_token(mem, directive_scanner, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_SENTINEL:
            directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_SENTINEL;
            break;
    }

    directive->source_location = location;
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

        case KEFIR_PREPROCESSOR_DIRECTIVE_INCLUDE:
        case KEFIR_PREPROCESSOR_DIRECTIVE_LINE:
        case KEFIR_PREPROCESSOR_DIRECTIVE_ERROR:
        case KEFIR_PREPROCESSOR_DIRECTIVE_PRAGMA:
        case KEFIR_PREPROCESSOR_DIRECTIVE_NON:
            REQUIRE_OK(kefir_token_buffer_free(mem, &directive->pp_tokens));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_PP_TOKEN:
            REQUIRE_OK(kefir_token_free(mem, &directive->pp_token));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_DEFINE:
            if (!directive->define_directive.object) {
                REQUIRE_OK(kefir_list_free(mem, &directive->define_directive.parameters));
            }
            REQUIRE_OK(kefir_token_buffer_free(mem, &directive->define_directive.replacement));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_UNDEF:
        case KEFIR_PREPROCESSOR_DIRECTIVE_IFDEF:
        case KEFIR_PREPROCESSOR_DIRECTIVE_IFNDEF:
        case KEFIR_PREPROCESSOR_DIRECTIVE_ELSE:
        case KEFIR_PREPROCESSOR_DIRECTIVE_ENDIF:
        case KEFIR_PREPROCESSOR_DIRECTIVE_EMPTY:
        case KEFIR_PREPROCESSOR_DIRECTIVE_SENTINEL:
            // Intentionally left blank
            break;
    }
    return KEFIR_OK;
}
