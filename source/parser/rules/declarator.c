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
#include "kefir/core/source_error.h"

static kefir_result_t scan_type_qualifier_list(struct kefir_mem *mem, struct kefir_parser *parser,
                                               struct kefir_ast_type_qualifier_list *type_qualifiers) {
    while (true) {
        if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_CONST)) {
            REQUIRE_OK(kefir_ast_type_qualifier_list_append(mem, type_qualifiers, KEFIR_AST_TYPE_QUALIFIER_CONST));
        } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_VOLATILE)) {
            REQUIRE_OK(kefir_ast_type_qualifier_list_append(mem, type_qualifiers, KEFIR_AST_TYPE_QUALIFIER_VOLATILE));
        } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_RESTRICT)) {
            REQUIRE_OK(kefir_ast_type_qualifier_list_append(mem, type_qualifiers, KEFIR_AST_TYPE_QUALIFIER_RESTRICT));
        } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_ATOMIC)) {
            REQUIRE_OK(kefir_ast_type_qualifier_list_append(mem, type_qualifiers, KEFIR_AST_TYPE_QUALIFIER_ATOMIC));
        } else {
            break;
        }

        REQUIRE_OK(PARSER_SHIFT(parser));
    }
    return KEFIR_OK;
}

static kefir_result_t scan_pointer(struct kefir_mem *mem, struct kefir_parser *parser,
                                   kefir_result_t (*scan_declarator)(struct kefir_mem *, struct kefir_parser *,
                                                                     struct kefir_ast_declarator **),
                                   struct kefir_ast_declarator **declarator_ptr) {
    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_STAR),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Cannot match AST pointer declarator"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    struct kefir_ast_type_qualifier_list type_qualifiers;
    REQUIRE_OK(kefir_ast_type_qualifier_list_init(&type_qualifiers));

    kefir_result_t res = scan_type_qualifier_list(mem, parser, &type_qualifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_type_qualifier_list_free(mem, &type_qualifiers);
        return res;
    });

    struct kefir_ast_declarator *subdeclarator = NULL;
    res = scan_declarator(mem, parser, &subdeclarator);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_type_qualifier_list_free(mem, &type_qualifiers);
        return res;
    });

    struct kefir_ast_declarator *declarator = kefir_ast_declarator_pointer(mem, subdeclarator);
    REQUIRE_ELSE(declarator != NULL, {
        kefir_ast_declarator_free(mem, subdeclarator);
        kefir_ast_type_qualifier_list_free(mem, &type_qualifiers);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST pointer declarator");
    });

    res = kefir_ast_type_qualifier_list_clone(mem, &declarator->pointer.type_qualifiers, &type_qualifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_free(mem, declarator);
        kefir_ast_type_qualifier_list_free(mem, &type_qualifiers);
        return res;
    });

    res = kefir_ast_type_qualifier_list_free(mem, &type_qualifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_free(mem, declarator);
        return res;
    });

    *declarator_ptr = declarator;
    return KEFIR_OK;
}

static kefir_result_t scan_direct_declarator_base(struct kefir_mem *mem, struct kefir_parser *parser,
                                                  struct kefir_ast_declarator **declarator_ptr) {
    kefir_result_t res = KEFIR_OK;
    struct kefir_ast_declarator *base_declarator = NULL;

    if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE)) {
        REQUIRE_CHAIN(&res, PARSER_SHIFT(parser));
        REQUIRE_CHAIN(&res, kefir_parser_scan_declarator(mem, parser, &base_declarator));
        REQUIRE_CHAIN_SET(
            &res, PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected right parenthese"));
        REQUIRE_CHAIN(&res, PARSER_SHIFT(parser));
    } else {
        REQUIRE_CHAIN_SET(&res, PARSER_TOKEN_IS_IDENTIFIER(parser, 0),
                          KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Expected identifier"));
        if (res == KEFIR_OK) {
            base_declarator = kefir_ast_declarator_identifier(
                mem, parser->symbols, kefir_parser_token_cursor_at(parser->cursor, 0)->identifier);
            REQUIRE_CHAIN_SET(&res, base_declarator != NULL,
                              KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST identifier declarator"));
            REQUIRE_CHAIN(&res, PARSER_SHIFT(parser));
        }
    }
    REQUIRE_ELSE(res == KEFIR_OK, {
        if (base_declarator != NULL) {
            kefir_ast_declarator_free(mem, base_declarator);
        }
        return res;
    });

    *declarator_ptr = base_declarator;
    return KEFIR_OK;
}

static kefir_result_t scan_array_impl(struct kefir_mem *mem, struct kefir_parser *parser,
                                      struct kefir_ast_type_qualifier_list *type_qualifiers,
                                      struct kefir_ast_declarator **declarator_ptr) {
    kefir_bool_t static_array = false;
    struct kefir_ast_node_base *length = NULL;
    kefir_ast_declarator_array_type_t array_type = KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED;

    REQUIRE_OK(PARSER_SHIFT(parser));
    if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_STATIC)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        static_array = true;
    }
    REQUIRE_OK(scan_type_qualifier_list(mem, parser, type_qualifiers));
    if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_STATIC)) {
        REQUIRE(!static_array,
                KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                       "Static specifier duplication in array declaration is forbidden"));
        REQUIRE_OK(PARSER_SHIFT(parser));
        static_array = true;
    }

    kefir_result_t res = KEFIR_OK;
    if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_STAR)) {
        REQUIRE(!static_array,
                KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                       "Static specifier combination with star in array declaration is forbidden"));
        REQUIRE_OK(PARSER_SHIFT(parser));
        array_type = KEFIR_AST_DECLARATOR_ARRAY_VLA_UNSPECIFIED;
    } else {
        res = KEFIR_PARSER_RULE_APPLY(mem, parser, assignment_expression, &length);
        if (res == KEFIR_NO_MATCH) {
            if (static_array) {
                res = KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                             "Static array declaration shall include length");
            } else {
                res = KEFIR_OK;
            }
        } else if (res == KEFIR_OK) {
            array_type = KEFIR_AST_DECLARATOR_ARRAY_BOUNDED;
        }
        REQUIRE_OK(res);
    }

    REQUIRE_ELSE(PARSER_TOKEN_IS_RIGHT_BRACKET(parser, 0), {
        if (length != NULL) {
            KEFIR_AST_NODE_FREE(mem, length);
        }
        return KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected right bracket");
    });
    res = PARSER_SHIFT(parser);
    REQUIRE_ELSE(res == KEFIR_OK, {
        if (length != NULL) {
            KEFIR_AST_NODE_FREE(mem, length);
        }
        return res;
    });

    struct kefir_ast_declarator *declarator = kefir_ast_declarator_array(mem, array_type, length, *declarator_ptr);
    REQUIRE_ELSE(declarator != NULL, {
        if (length != NULL) {
            KEFIR_AST_NODE_FREE(mem, length);
        }
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST array declarator");
    });
    declarator->array.static_array = static_array;
    *declarator_ptr = NULL;

    res = kefir_ast_type_qualifier_list_clone(mem, &declarator->array.type_qualifiers, type_qualifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_free(mem, declarator);
        return res;
    });

    *declarator_ptr = declarator;
    return KEFIR_OK;
}

static kefir_result_t scan_array(struct kefir_mem *mem, struct kefir_parser *parser,
                                 struct kefir_ast_declarator **declarator_ptr) {
    struct kefir_ast_type_qualifier_list type_qualifiers;
    REQUIRE_OK(kefir_ast_type_qualifier_list_init(&type_qualifiers));
    kefir_result_t res = scan_array_impl(mem, parser, &type_qualifiers, declarator_ptr);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_type_qualifier_list_free(mem, &type_qualifiers);
        return res;
    });
    res = kefir_ast_type_qualifier_list_free(mem, &type_qualifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_free(mem, *declarator_ptr);
        *declarator_ptr = NULL;
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t scan_function_parameter_declarator(struct kefir_mem *mem, struct kefir_parser *parser,
                                                         struct kefir_ast_declarator **declarator_ptr) {
    struct kefir_ast_declarator *declarator = NULL;
    kefir_size_t checkpoint;
    REQUIRE_OK(kefir_parser_token_cursor_save(parser->cursor, &checkpoint));

    kefir_result_t res = kefir_parser_scan_declarator(mem, parser, &declarator);
    if (res == KEFIR_NO_MATCH) {
        REQUIRE_OK(kefir_parser_token_cursor_restore(parser->cursor, checkpoint));

        res = kefir_parser_scan_abstract_declarator(mem, parser, &declarator);
        if (res == KEFIR_NO_MATCH) {
            REQUIRE_OK(kefir_parser_token_cursor_restore(parser->cursor, checkpoint));
            declarator = kefir_ast_declarator_identifier(mem, NULL, NULL);
            REQUIRE(declarator != NULL,
                    KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate abstract AST identifier declarator"));
            res = KEFIR_OK;
        }
    }

    if (res == KEFIR_OK) {
        *declarator_ptr = declarator;
    }
    return res;
}

static kefir_result_t scan_function_parameter(struct kefir_mem *mem, struct kefir_parser *parser,
                                              struct kefir_ast_declarator *func_declarator) {
    struct kefir_ast_declarator_specifier_list specifiers;
    REQUIRE_OK(kefir_ast_declarator_specifier_list_init(&specifiers));
    kefir_result_t res = kefir_parser_scan_declaration_specifier_list(mem, parser, &specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_list_free(mem, &specifiers);
        return res;
    });

    struct kefir_source_location source_location = kefir_parser_token_cursor_at(parser->cursor, 0)->source_location;
    struct kefir_ast_declarator *declarator = NULL;
    res = scan_function_parameter_declarator(mem, parser, &declarator);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_list_free(mem, &specifiers);
        return res;
    });

    struct kefir_ast_init_declarator *init_declarator = NULL;
    struct kefir_ast_declaration *declaration =
        kefir_ast_new_single_declaration(mem, declarator, NULL, &init_declarator);
    REQUIRE_ELSE(declaration != NULL, {
        kefir_ast_declarator_free(mem, declarator);
        kefir_ast_declarator_specifier_list_free(mem, &specifiers);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST declaration");
    });
    init_declarator->base.source_location = source_location;
    declaration->base.source_location = source_location;

    res = kefir_ast_declarator_specifier_list_clone(mem, &declaration->specifiers, &specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(declaration));
        kefir_ast_declarator_specifier_list_free(mem, &specifiers);
        return res;
    });

    res = kefir_ast_declarator_specifier_list_free(mem, &specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(declaration));
        return res;
    });

    res = kefir_list_insert_after(mem, &func_declarator->function.parameters,
                                  kefir_list_tail(&func_declarator->function.parameters),
                                  KEFIR_AST_NODE_BASE(declaration));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(declaration));
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t scan_function_parameter_list(struct kefir_mem *mem, struct kefir_parser *parser,
                                                   struct kefir_ast_declarator *declarator) {
    REQUIRE_OK(scan_function_parameter(mem, parser, declarator));
    kefir_bool_t scan_parameters = true;
    while (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COMMA) && scan_parameters) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        kefir_result_t res = scan_function_parameter(mem, parser, declarator);
        if (res == KEFIR_NO_MATCH) {
            REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_ELLIPSIS),
                    KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                           "Expected either function parameter declaration or ellipsis"));
            REQUIRE_OK(PARSER_SHIFT(parser));
            declarator->function.ellipsis = true;
            scan_parameters = false;
        } else {
            REQUIRE_OK(res);
        }
    }

    return KEFIR_OK;
}

static kefir_result_t scan_function_identifier_list(struct kefir_mem *mem, struct kefir_parser *parser,
                                                    struct kefir_ast_declarator *declarator) {
    kefir_bool_t scan_parameters = PARSER_TOKEN_IS_IDENTIFIER(parser, 0);
    while (scan_parameters) {
        struct kefir_ast_node_base *param = NULL;
        REQUIRE_OK(KEFIR_PARSER_RULE_APPLY(mem, parser, identifier, &param));
        kefir_result_t res = kefir_list_insert_after(mem, &declarator->function.parameters,
                                                     kefir_list_tail(&declarator->function.parameters), param);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_AST_NODE_FREE(mem, param);
            return res;
        });

        if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COMMA)) {
            REQUIRE_OK(PARSER_SHIFT(parser));
            scan_parameters = true;
        } else {
            scan_parameters = false;
        }
    }
    return KEFIR_OK;
}

static kefir_result_t scan_function_impl(struct kefir_mem *mem, struct kefir_parser *parser, kefir_bool_t abstract,
                                         struct kefir_ast_declarator *declarator) {
    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Expected left parenthese"));
    REQUIRE_OK(PARSER_SHIFT(parser));

    kefir_size_t checkpoint;
    REQUIRE_OK(kefir_parser_token_cursor_save(parser->cursor, &checkpoint));

    kefir_result_t res = scan_function_parameter_list(mem, parser, declarator);
    if (res == KEFIR_NO_MATCH) {
        REQUIRE_OK(kefir_parser_token_cursor_restore(parser->cursor, checkpoint));
        if (abstract) {
            res = KEFIR_OK;
        } else {
            res = scan_function_identifier_list(mem, parser, declarator);
        }
    }
    REQUIRE_OK(res);
    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Expected right parenthese"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    return KEFIR_OK;
}

static kefir_result_t scan_function(struct kefir_mem *mem, struct kefir_parser *parser, kefir_bool_t abstract,
                                    struct kefir_ast_declarator **declarator_ptr) {
    struct kefir_ast_declarator *declarator = kefir_ast_declarator_function(mem, *declarator_ptr);
    kefir_result_t res = scan_function_impl(mem, parser, abstract, declarator);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_free(mem, declarator);
        *declarator_ptr = NULL;
        return res;
    });
    *declarator_ptr = declarator;
    return KEFIR_OK;
}

static kefir_result_t scan_direct_declarator_tail(struct kefir_mem *mem, struct kefir_parser *parser,
                                                  kefir_bool_t abstract, struct kefir_ast_declarator **declarator_ptr) {
    kefir_result_t res = KEFIR_OK;
    kefir_bool_t scan_declarators = true;
    while (scan_declarators && res == KEFIR_OK) {
        if (PARSER_TOKEN_IS_LEFT_BRACKET(parser, 0)) {
            res = scan_array(mem, parser, declarator_ptr);
        } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE)) {
            res = scan_function(mem, parser, abstract, declarator_ptr);
        } else {
            scan_declarators = false;
        }
    }

    REQUIRE_ELSE(res == KEFIR_OK, {
        if (*declarator_ptr != NULL) {
            kefir_ast_declarator_free(mem, *declarator_ptr);
        }
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t scan_direct(struct kefir_mem *mem, struct kefir_parser *parser,
                                  struct kefir_ast_declarator **declarator_ptr) {
    struct kefir_ast_declarator *base_declarator = NULL;
    REQUIRE_OK(scan_direct_declarator_base(mem, parser, &base_declarator));
    REQUIRE_OK(scan_direct_declarator_tail(mem, parser, false, &base_declarator));
    *declarator_ptr = base_declarator;
    return KEFIR_OK;
}

kefir_result_t kefir_parser_scan_declarator(struct kefir_mem *mem, struct kefir_parser *parser,
                                            struct kefir_ast_declarator **declarator_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser"));
    REQUIRE(declarator_ptr != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to declarator"));

    kefir_size_t checkpoint;
    kefir_result_t res = KEFIR_OK;
    REQUIRE_OK(kefir_parser_token_cursor_save(parser->cursor, &checkpoint));
    if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_STAR)) {
        res = scan_pointer(mem, parser, kefir_parser_scan_declarator, declarator_ptr);
    } else {
        res = scan_direct(mem, parser, declarator_ptr);
    }

    if (res == KEFIR_NO_MATCH) {
        REQUIRE_OK(kefir_parser_token_cursor_restore(parser->cursor, checkpoint));
    } else {
        REQUIRE_OK(res);
    }

    return res;
}

static kefir_result_t scan_direct_abstract_declarator_base(struct kefir_mem *mem, struct kefir_parser *parser,
                                                           struct kefir_ast_declarator **declarator_ptr) {
    struct kefir_ast_declarator *base_declarator = NULL;

    kefir_size_t checkpoint;
    REQUIRE_OK(kefir_parser_token_cursor_save(parser->cursor, &checkpoint));

    kefir_result_t res = KEFIR_NO_MATCH;
    if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE)) {
        res = KEFIR_OK;
        REQUIRE_CHAIN(&res, PARSER_SHIFT(parser));
        REQUIRE_CHAIN(&res, kefir_parser_scan_abstract_declarator(mem, parser, &base_declarator));
        REQUIRE_CHAIN_SET(
            &res, PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected right parenthese"));
        REQUIRE_CHAIN(&res, PARSER_SHIFT(parser));
    }

    if (res == KEFIR_NO_MATCH) {
        res = kefir_parser_token_cursor_restore(parser->cursor, checkpoint);
        if (res == KEFIR_OK) {
            base_declarator = kefir_ast_declarator_identifier(mem, NULL, NULL);
            REQUIRE_CHAIN_SET(
                &res, base_declarator != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate empty AST abstract declarator"));
        }
    }
    REQUIRE_ELSE(res == KEFIR_OK, {
        if (base_declarator != NULL) {
            kefir_ast_declarator_free(mem, base_declarator);
        }
        return res;
    });

    *declarator_ptr = base_declarator;
    return KEFIR_OK;
}

static kefir_result_t scan_direct_abstract(struct kefir_mem *mem, struct kefir_parser *parser,
                                           kefir_bool_t permit_empty, struct kefir_ast_declarator **declarator_ptr) {
    struct kefir_ast_declarator *base_declarator = NULL;
    REQUIRE_OK(scan_direct_abstract_declarator_base(mem, parser, &base_declarator));
    REQUIRE_OK(scan_direct_declarator_tail(mem, parser, true, &base_declarator));
    if (base_declarator->klass == KEFIR_AST_DECLARATOR_IDENTIFIER && base_declarator->identifier == NULL) {
        REQUIRE_ELSE(permit_empty, {
            kefir_ast_declarator_free(mem, base_declarator);
            return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to parse empty abstract declarator");
        });
    }
    *declarator_ptr = base_declarator;
    return KEFIR_OK;
}

static kefir_result_t scan_abstract_declarator_pointer_tail(struct kefir_mem *mem, struct kefir_parser *parser,
                                                            struct kefir_ast_declarator **declarator_ptr) {

    kefir_result_t res = scan_pointer(mem, parser, scan_abstract_declarator_pointer_tail, declarator_ptr);
    if (res == KEFIR_NO_MATCH) {
        res = scan_direct_abstract(mem, parser, true, declarator_ptr);
    }
    REQUIRE_OK(res);
    return KEFIR_OK;
}

kefir_result_t kefir_parser_scan_abstract_declarator(struct kefir_mem *mem, struct kefir_parser *parser,
                                                     struct kefir_ast_declarator **declarator_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser"));
    REQUIRE(declarator_ptr != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to declarator"));

    kefir_size_t checkpoint;
    kefir_result_t res = KEFIR_OK;
    REQUIRE_OK(kefir_parser_token_cursor_save(parser->cursor, &checkpoint));
    if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_STAR)) {
        res = scan_pointer(mem, parser, scan_abstract_declarator_pointer_tail, declarator_ptr);
    } else {
        res = scan_direct_abstract(mem, parser, false, declarator_ptr);
    }

    if (res == KEFIR_NO_MATCH) {
        REQUIRE_OK(kefir_parser_token_cursor_restore(parser->cursor, checkpoint));
    } else {
        REQUIRE_OK(res);
    }

    return res;
}
