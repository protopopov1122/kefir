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

static kefir_result_t skip_whitespaces(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                       struct kefir_token *token) {
    kefir_bool_t skip_whitespaces = true;
    while (skip_whitespaces) {
        REQUIRE_OK(kefir_preprocessor_tokenize_next(mem, &preprocessor->lexer, token));
        if (token->klass != KEFIR_TOKEN_PP_WHITESPACE || token->pp_whitespace.newline) {
            skip_whitespaces = false;
        } else {
            REQUIRE_OK(kefir_token_free(mem, token));
        }
    }
    return KEFIR_OK;
}

static kefir_result_t scan_pp_tokens(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                     struct kefir_token_buffer *buffer) {
    struct kefir_token next_token;
    kefir_result_t scan_tokens = true;
    while (scan_tokens) {
        REQUIRE_OK(kefir_preprocessor_tokenize_next(mem, &preprocessor->lexer, &next_token));

        if (next_token.klass == KEFIR_TOKEN_SENTINEL ||
            (next_token.klass == KEFIR_TOKEN_PP_WHITESPACE && next_token.pp_whitespace.newline)) {
            scan_tokens = false;
            REQUIRE_OK(kefir_token_free(mem, &next_token));
        } else {
            REQUIRE_OK(kefir_token_buffer_emplace(mem, buffer, &next_token));
        }
    }
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

kefir_result_t kefir_preprocessor_match_directive(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                                  kefir_preprocessor_directive_type_t *directive_type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));
    REQUIRE(directive_type != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to directive type"));

    struct kefir_lexer_source_cursor_state initial_state, hash_state;
    REQUIRE_OK(kefir_lexer_source_cursor_save(preprocessor->lexer.cursor, &initial_state));

    REQUIRE_OK(skip_whitespaces_until(preprocessor->lexer.cursor, preprocessor->lexer.context->newline));
    kefir_char32_t chr = kefir_lexer_source_cursor_at(preprocessor->lexer.cursor, 0);
    if (chr == U'\0') {
        *directive_type = KEFIR_PREPROCESSOR_DIRECTIVE_SENTINEL;
        return KEFIR_OK;
    }
    if (chr != U'#') {
        REQUIRE_OK(kefir_lexer_source_cursor_restore(preprocessor->lexer.cursor, &initial_state));
        *directive_type = KEFIR_PREPROCESSOR_DIRECTIVE_TEXT_LINE;
        return KEFIR_OK;
    }
    REQUIRE_OK(kefir_lexer_source_cursor_save(preprocessor->lexer.cursor, &hash_state));
    REQUIRE_OK(kefir_lexer_source_cursor_next(preprocessor->lexer.cursor, 1));
    REQUIRE_OK(skip_whitespaces_until(preprocessor->lexer.cursor, preprocessor->lexer.context->newline));
    chr = kefir_lexer_source_cursor_at(preprocessor->lexer.cursor, 0);
    if (chr == preprocessor->lexer.context->newline) {
        *directive_type = KEFIR_PREPROCESSOR_DIRECTIVE_EMPTY;
        return KEFIR_OK;
    }

#define MAX_DIRECTIVE_NAME 64
    kefir_char32_t directive_name[MAX_DIRECTIVE_NAME] = {0};
    kefir_size_t directive_name_idx = 0;
    while (kefir_isnondigit32(chr) && directive_name_idx + 1 < MAX_DIRECTIVE_NAME) {
        directive_name[directive_name_idx++] = chr;
        REQUIRE_OK(kefir_lexer_source_cursor_next(preprocessor->lexer.cursor, 1));
        chr = kefir_lexer_source_cursor_at(preprocessor->lexer.cursor, 0);
    }
#undef MAX_DIRECTIVE_NAME

    static const struct {
        const kefir_char32_t *literal;
        kefir_preprocessor_directive_type_t directive;
    } KnownDirectives[] = {
        {U"if", KEFIR_PREPROCESSOR_DIRECTIVE_IF},          {U"ifdef", KEFIR_PREPROCESSOR_DIRECTIVE_IFDEF},
        {U"ifndef", KEFIR_PREPROCESSOR_DIRECTIVE_IFNDEF},  {U"elif", KEFIR_PREPROCESSOR_DIRECTIVE_ELIF},
        {U"else", KEFIR_PREPROCESSOR_DIRECTIVE_ELSE},      {U"endif", KEFIR_PREPROCESSOR_DIRECTIVE_ENDIF},
        {U"include", KEFIR_PREPROCESSOR_DIRECTIVE_INCLUDE}};
    for (kefir_size_t i = 0; i < sizeof(KnownDirectives) / sizeof(KnownDirectives[0]); i++) {
        if (kefir_strcmp32(KnownDirectives[i].literal, directive_name) == 0) {
            *directive_type = KnownDirectives[i].directive;
            return KEFIR_OK;
        }
    }

    REQUIRE_OK(kefir_lexer_source_cursor_restore(preprocessor->lexer.cursor, &hash_state));
    *directive_type = KEFIR_PREPROCESSOR_DIRECTIVE_NON;
    return KEFIR_OK;
}

static kefir_result_t next_if(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                              struct kefir_preprocessor_directive *directive) {
    directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_IF;
    REQUIRE_OK(kefir_token_buffer_init(mem, &directive->if_directive.condition));

    kefir_result_t res = scan_pp_tokens(mem, preprocessor, &directive->if_directive.condition);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &directive->if_directive.condition);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t next_elif(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                struct kefir_preprocessor_directive *directive) {
    directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_ELIF;
    REQUIRE_OK(kefir_token_buffer_init(mem, &directive->if_directive.condition));

    kefir_result_t res = scan_pp_tokens(mem, preprocessor, &directive->if_directive.condition);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &directive->if_directive.condition);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t next_ifdef(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                 struct kefir_preprocessor_directive *directive, kefir_bool_t inverse) {
    struct kefir_token token;
    REQUIRE_OK(skip_whitespaces(mem, preprocessor, &token));
    REQUIRE_ELSE(token.klass == KEFIR_TOKEN_IDENTIFIER, {
        kefir_token_free(mem, &token);
        return KEFIR_SET_SOURCE_ERROR(KEFIR_LEXER_ERROR, &token.source_location, "Expected identifier");
    });
    const char *identifier = kefir_symbol_table_insert(mem, preprocessor->lexer.symbols, token.identifier, NULL);
    REQUIRE_ELSE(identifier != NULL, {
        kefir_token_free(mem, &token);
        return KEFIR_SET_ERROR(KEFIR_OBJALLOC_FAILURE, "Failed to insert identifier into symbol table");
    });
    REQUIRE_OK(kefir_token_free(mem, &token));
    REQUIRE_OK(kefir_preprocessor_skip_line(preprocessor));

    directive->type = inverse ? KEFIR_PREPROCESSOR_DIRECTIVE_IFNDEF : KEFIR_PREPROCESSOR_DIRECTIVE_IFDEF;
    directive->ifdef_directive.identifier = identifier;
    return KEFIR_OK;
}

static kefir_result_t next_else(struct kefir_preprocessor *preprocessor,
                                struct kefir_preprocessor_directive *directive) {
    directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_ELSE;
    REQUIRE_OK(kefir_preprocessor_skip_line(preprocessor));
    return KEFIR_OK;
}

static kefir_result_t next_endif(struct kefir_preprocessor *preprocessor,
                                 struct kefir_preprocessor_directive *directive) {
    directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_ENDIF;
    REQUIRE_OK(kefir_preprocessor_skip_line(preprocessor));
    return KEFIR_OK;
}

static kefir_result_t next_include(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                   struct kefir_preprocessor_directive *directive) {
    directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_INCLUDE;
    REQUIRE_OK(kefir_token_buffer_init(mem, &directive->pp_tokens.pp_tokens));

    struct kefir_lexer_source_cursor_state state;
    struct kefir_token token;
    kefir_result_t res = kefir_lexer_source_cursor_save(preprocessor->lexer.cursor, &state);
    REQUIRE_CHAIN(&res, skip_whitespaces_until(preprocessor->lexer.cursor, preprocessor->lexer.context->newline));
    REQUIRE_CHAIN(&res, kefir_lexer_match_pp_header_name(mem, &preprocessor->lexer, &token));
    if (res == KEFIR_NO_MATCH) {
        res = kefir_lexer_source_cursor_restore(preprocessor->lexer.cursor, &state);
        REQUIRE_CHAIN(&res, scan_pp_tokens(mem, preprocessor, &directive->pp_tokens.pp_tokens));
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_buffer_free(mem, &directive->pp_tokens.pp_tokens);
            return res;
        });
    } else {
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_buffer_free(mem, &directive->pp_tokens.pp_tokens);
            return res;
        });
        res = kefir_token_buffer_emplace(mem, &directive->pp_tokens.pp_tokens, &token);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_free(mem, &token);
            kefir_token_buffer_free(mem, &directive->pp_tokens.pp_tokens);
            return res;
        });
        res = kefir_preprocessor_skip_line(preprocessor);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_buffer_free(mem, &directive->pp_tokens.pp_tokens);
            return res;
        });
    }
    return KEFIR_OK;
}

static kefir_result_t next_non_directive(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                         struct kefir_preprocessor_directive *directive) {
    directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_NON;
    REQUIRE_OK(kefir_token_buffer_init(mem, &directive->pp_tokens.pp_tokens));

    kefir_result_t res = scan_pp_tokens(mem, preprocessor, &directive->pp_tokens.pp_tokens);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_token_buffer_free(mem, &directive->pp_tokens.pp_tokens);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t next_text_line(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                     struct kefir_preprocessor_directive *directive) {
    directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_TEXT_LINE;
    REQUIRE_OK(kefir_token_buffer_init(mem, &directive->pp_tokens.pp_tokens));

    struct kefir_token token;
    kefir_bool_t scan_tokens = true;
    while (scan_tokens) {
        kefir_result_t res = kefir_preprocessor_tokenize_next(mem, &preprocessor->lexer, &token);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_token_buffer_free(mem, &directive->pp_tokens.pp_tokens);
            return res;
        });

        if (token.klass != KEFIR_TOKEN_SENTINEL) {
            scan_tokens = token.klass != KEFIR_TOKEN_PP_WHITESPACE || !token.pp_whitespace.newline;
            res = kefir_token_buffer_emplace(mem, &directive->pp_tokens.pp_tokens, &token);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_token_free(mem, &token);
                kefir_token_buffer_free(mem, &directive->pp_tokens.pp_tokens);
                return res;
            });
        } else {
            res = kefir_token_free(mem, &token);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_token_buffer_free(mem, &directive->pp_tokens.pp_tokens);
                return res;
            });
            scan_tokens = false;
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_next_directive(struct kefir_mem *mem, struct kefir_preprocessor *preprocessor,
                                                 struct kefir_preprocessor_directive *directive) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(preprocessor != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor"));
    REQUIRE(directive != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor directive"));

    kefir_preprocessor_directive_type_t directive_type;
    REQUIRE_OK(kefir_preprocessor_match_directive(mem, preprocessor, &directive_type));
    switch (directive_type) {
        case KEFIR_PREPROCESSOR_DIRECTIVE_IF:
            REQUIRE_OK(next_if(mem, preprocessor, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_IFDEF:
            REQUIRE_OK(next_ifdef(mem, preprocessor, directive, false));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_IFNDEF:
            REQUIRE_OK(next_ifdef(mem, preprocessor, directive, true));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_ELIF:
            REQUIRE_OK(next_elif(mem, preprocessor, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_ELSE:
            REQUIRE_OK(next_else(preprocessor, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_ENDIF:
            REQUIRE_OK(next_endif(preprocessor, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_INCLUDE:
            REQUIRE_OK(next_include(mem, preprocessor, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_DEFINE_OBJECT:
        case KEFIR_PREPROCESSOR_DIRECTIVE_DEFINE_FUNCTION:
        case KEFIR_PREPROCESSOR_DIRECTIVE_UNDEF:
        case KEFIR_PREPROCESSOR_DIRECTIVE_LINE:
        case KEFIR_PREPROCESSOR_DIRECTIVE_ERROR:
        case KEFIR_PREPROCESSOR_DIRECTIVE_PRAGMA:
            return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Preprocessor directive is not implemented yet");

        case KEFIR_PREPROCESSOR_DIRECTIVE_EMPTY:
            directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_EMPTY;
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_NON:
            REQUIRE_OK(next_non_directive(mem, preprocessor, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_TEXT_LINE:
            REQUIRE_OK(next_text_line(mem, preprocessor, directive));
            break;

        case KEFIR_PREPROCESSOR_DIRECTIVE_SENTINEL:
            directive->type = KEFIR_PREPROCESSOR_DIRECTIVE_SENTINEL;
            break;
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
