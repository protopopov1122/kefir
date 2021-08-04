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

static kefir_result_t scan_storage_class(struct kefir_mem *mem, struct kefir_parser *parser, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct kefir_ast_declarator_specifier_list *, specifiers, payload);
    struct kefir_ast_declarator_specifier *specifier = NULL;
    if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_TYPEDEF)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_storage_class_specifier_typedef(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_EXTERN)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_storage_class_specifier_extern(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_STATIC)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_storage_class_specifier_static(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_THREAD_LOCAL)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_storage_class_specifier_thread_local(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_AUTO)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_storage_class_specifier_auto(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_REGISTER)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_storage_class_specifier_register(mem);
    } else {
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match storage class specifier");
    }

    REQUIRE(specifier != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST declarator specifier"));
    kefir_result_t res = kefir_ast_declarator_specifier_list_append(mem, specifiers, specifier);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_free(mem, specifier);
        return res;
    });

    return KEFIR_OK;
}

static kefir_result_t scan_type_specifier(struct kefir_mem *mem, struct kefir_parser *parser, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct kefir_ast_declarator_specifier_list *, specifiers, payload);
    struct kefir_ast_declarator_specifier *specifier = NULL;
    if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_VOID)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_type_specifier_void(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_CHAR)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_type_specifier_char(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_SHORT)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_type_specifier_short(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_INT)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_type_specifier_int(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_LONG)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_type_specifier_long(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_FLOAT)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_type_specifier_float(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_DOUBLE)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_type_specifier_double(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_SIGNED)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_type_specifier_signed(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_UNSIGNED)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_type_specifier_unsigned(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_BOOL)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_type_specifier_bool(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_COMPLEX)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_type_specifier_complex(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_ATOMIC) &&
               PARSER_TOKEN_IS_PUNCTUATOR(parser, 1, KEFIR_PUNCTUATOR_LEFT_PARENTHESE)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        REQUIRE_OK(PARSER_SHIFT(parser));
        struct kefir_ast_node_base *type_name = NULL;
        REQUIRE_OK(KEFIR_PARSER_RULE_APPLY(mem, parser, type_name, &type_name));
        specifier = kefir_ast_type_specifier_atomic(mem, type_name);
        REQUIRE_ELSE(specifier != NULL, {
            KEFIR_AST_NODE_FREE(mem, type_name);
            return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST declarator specifier");
        });
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_STRUCT)) {
        return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Structure type specifier is not implemented yet");
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_UNION)) {
        return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Union type specifier is not implemented yet");
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_ENUM)) {
        return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Enumeration type specifier is not implemented yet");
    } else {
        // TODO Implement typedef specifiers
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match type specifier");
    }

    REQUIRE(specifier != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST declarator specifier"));
    kefir_result_t res = kefir_ast_declarator_specifier_list_append(mem, specifiers, specifier);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_free(mem, specifier);
        return res;
    });

    return KEFIR_OK;
}

static kefir_result_t scan_type_qualifier(struct kefir_mem *mem, struct kefir_parser *parser, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct kefir_ast_declarator_specifier_list *, specifiers, payload);
    struct kefir_ast_declarator_specifier *specifier = NULL;
    if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_CONST)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_type_qualifier_const(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_RESTRICT)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_type_qualifier_restrict(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_VOLATILE)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_type_qualifier_volatile(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_ATOMIC)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_type_qualifier_atomic(mem);
    } else {
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match type qualifier");
    }

    REQUIRE(specifier != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST declarator specifier"));
    kefir_result_t res = kefir_ast_declarator_specifier_list_append(mem, specifiers, specifier);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_free(mem, specifier);
        return res;
    });

    return KEFIR_OK;
}

static kefir_result_t scan_function_specifier(struct kefir_mem *mem, struct kefir_parser *parser, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct kefir_ast_declarator_specifier_list *, specifiers, payload);
    struct kefir_ast_declarator_specifier *specifier = NULL;
    if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_INLINE)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_function_specifier_inline(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_NORETURN)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_function_specifier_noreturn(mem);
    } else {
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match function specifier");
    }

    REQUIRE(specifier != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST declarator specifier"));
    kefir_result_t res = kefir_ast_declarator_specifier_list_append(mem, specifiers, specifier);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_free(mem, specifier);
        return res;
    });

    return KEFIR_OK;
}

static kefir_result_t scan_alignment_specifier(struct kefir_mem *mem, struct kefir_parser *parser, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct kefir_ast_declarator_specifier_list *, specifiers, payload);
    REQUIRE(PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_ALIGNAS),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match alignment specifier"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE),
            KEFIR_SET_ERROR(KEFIR_SYNTAX_ERROR, "Expected left parenthese"));
    REQUIRE_OK(PARSER_SHIFT(parser));

    struct kefir_ast_node_base *alignment = NULL;
    kefir_result_t res = KEFIR_PARSER_RULE_APPLY(mem, parser, constant_expression, &alignment);
    if (res == KEFIR_NO_MATCH) {
        res = KEFIR_PARSER_RULE_APPLY(mem, parser, type_name, &alignment);
    }
    REQUIRE_OK(res);
    REQUIRE_ELSE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE), {
        KEFIR_AST_NODE_FREE(mem, alignment);
        return KEFIR_SET_ERROR(KEFIR_SYNTAX_ERROR, "Expected right parenthese");
    });
    res = PARSER_SHIFT(parser);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, alignment);
        return res;
    });

    struct kefir_ast_declarator_specifier *specifier = kefir_ast_alignment_specifier(mem, alignment);
    REQUIRE_ELSE(specifier != NULL, {
        KEFIR_AST_NODE_FREE(mem, alignment);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST declarator specifier");
    });
    res = kefir_ast_declarator_specifier_list_append(mem, specifiers, specifier);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_free(mem, specifier);
        return res;
    });

    return KEFIR_OK;
}

kefir_result_t kefir_parser_scan_declaration_specifier(struct kefir_mem *mem, struct kefir_parser *parser,
                                                       struct kefir_ast_declarator_specifier_list *specifiers) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));
    REQUIRE(specifiers != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid declarator specifier list"));

    kefir_result_t res = kefir_parser_try_invoke(mem, parser, scan_storage_class, specifiers);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = kefir_parser_try_invoke(mem, parser, scan_type_specifier, specifiers);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = kefir_parser_try_invoke(mem, parser, scan_type_qualifier, specifiers);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    res = kefir_parser_try_invoke(mem, parser, scan_function_specifier, specifiers);
    REQUIRE(res == KEFIR_NO_MATCH, res);
    REQUIRE_OK(kefir_parser_try_invoke(mem, parser, scan_alignment_specifier, specifiers));

    return KEFIR_OK;
}

kefir_result_t kefir_parser_scan_declaration_specifier_list(struct kefir_mem *mem, struct kefir_parser *parser,
                                                            struct kefir_ast_declarator_specifier_list *specifiers) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));
    REQUIRE(specifiers != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid declarator specifier list"));

    kefir_bool_t scan_specifiers = true;
    while (scan_specifiers) {
        kefir_result_t res = kefir_parser_scan_declaration_specifier(mem, parser, specifiers);
        if (res == KEFIR_NO_MATCH) {
            scan_specifiers = false;
        } else {
            REQUIRE_OK(res);
        }
    }
    REQUIRE(kefir_list_length(&specifiers->list) > 0,
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Declarator specifier list cannot be empty"));
    return KEFIR_OK;
}
