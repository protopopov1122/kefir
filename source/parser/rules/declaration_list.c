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

#include "kefir/parser/rule_helpers.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/parser/builder.h"

static kefir_result_t scan_specifiers(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder) {
    struct kefir_ast_declarator_specifier_list list;
    REQUIRE_OK(kefir_ast_declarator_specifier_list_init(&list));
    kefir_result_t res = kefir_parser_scan_declaration_specifier_list(mem, builder->parser, &list);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_list_free(mem, &list);
        return res;
    });
    res = kefir_parser_ast_builder_declaration_list(mem, builder, &list);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_list_free(mem, &list);
        return res;
    });
    REQUIRE_OK(kefir_ast_declarator_specifier_list_free(mem, &list));
    return KEFIR_OK;
}

static kefir_result_t scan_init_declaration(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder) {
    struct kefir_ast_declarator *declarator = NULL;
    struct kefir_ast_initializer *initializer = NULL;
    kefir_result_t res = KEFIR_OK;

    REQUIRE_OK(kefir_parser_scan_declarator(mem, builder->parser, &declarator));
    if (PARSER_TOKEN_IS_PUNCTUATOR(builder->parser, 0, KEFIR_PUNCTUATOR_ASSIGN)) {
        REQUIRE_CHAIN(&res, PARSER_SHIFT(builder->parser));
        REQUIRE_CHAIN(&res, kefir_parser_scan_initializer(mem, builder->parser, &initializer));
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_declarator_free(mem, declarator);
            return res;
        });
    }

    res = kefir_parser_ast_builder_declaration(mem, builder, declarator, initializer);
    REQUIRE_ELSE(res == KEFIR_OK, {
        if (initializer != NULL) {
            kefir_ast_initializer_free(mem, initializer);
        }
        kefir_ast_declarator_free(mem, declarator);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t builder_callback(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder, void *payload) {
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser AST builder"));
    struct kefir_parser *parser = builder->parser;

    kefir_result_t res =
        kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(parser, static_assertion), NULL);
    REQUIRE(res == KEFIR_NO_MATCH, res);

    REQUIRE_OK(scan_specifiers(mem, builder));
    kefir_bool_t scan_init_decl = !PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_SEMICOLON);
    if (!scan_init_decl) {
        REQUIRE_OK(kefir_parser_ast_builder_declaration(mem, builder, NULL, NULL));
    }

    while (scan_init_decl) {
        REQUIRE_OK(scan_init_declaration(mem, builder));
        if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COMMA)) {
            scan_init_decl = true;
            REQUIRE_OK(PARSER_SHIFT(parser));
        } else {
            scan_init_decl = false;
        }
    }

    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_SEMICOLON),
            KEFIR_SET_ERROR(KEFIR_SYNTAX_ERROR, "Expected semiolon"));
    REQUIRE_OK(PARSER_SHIFT(parser));

    return KEFIR_OK;
}

static kefir_result_t update_scope(struct kefir_mem *mem, struct kefir_parser *parser,
                                   struct kefir_ast_declaration_list *declaration_list) {
    kefir_bool_t is_typedef = false;
    struct kefir_ast_declarator_specifier *specifier = NULL;
    kefir_result_t res = KEFIR_OK;
    for (struct kefir_list_entry *iter =
             kefir_ast_declarator_specifier_list_iter(&declaration_list->specifiers, &specifier);
         !is_typedef && iter != NULL && res == KEFIR_OK;
         res = kefir_ast_declarator_specifier_list_next(&iter, &specifier)) {
        if (specifier->klass == KEFIR_AST_STORAGE_CLASS_SPECIFIER &&
            specifier->storage_class == KEFIR_AST_STORAGE_SPECIFIER_TYPEDEF) {
            is_typedef = true;
        }
    }
    REQUIRE_OK(res);

    if (is_typedef) {
        for (const struct kefir_list_entry *iter = kefir_list_head(&declaration_list->declarations); iter != NULL;
             kefir_list_next(&iter)) {
            ASSIGN_DECL_CAST(struct kefir_ast_declaration *, declaration, iter->value);
            const char *identifier;
            REQUIRE_OK(kefir_ast_declarator_unpack_identifier(declaration->declarator, &identifier));
            if (identifier != NULL) {
                kefir_parser_scope_declare_typedef(mem, &parser->scope, identifier);
            }
        }
    }
    return KEFIR_OK;
}

kefir_result_t KEFIR_PARSER_RULE_FN_PREFIX(declaration_list)(struct kefir_mem *mem, struct kefir_parser *parser,
                                                             struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    REQUIRE_OK(kefir_parser_ast_builder_wrap(mem, parser, result, builder_callback, NULL));
    if ((*result)->klass->type == KEFIR_AST_DECLARATION_LIST) {
        ASSIGN_DECL_CAST(struct kefir_ast_declaration_list *, decl_list, (*result)->self);
        REQUIRE_OK(update_scope(mem, parser, decl_list));
    } else {
        REQUIRE((*result)->klass->type == KEFIR_AST_STATIC_ASSERTION,
                KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                                "Expected parser rule to produce either a declaration list, or a static assertion"));
    }
    return KEFIR_OK;
}
