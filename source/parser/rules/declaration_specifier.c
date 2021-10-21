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
#include "kefir/parser/builtins.h"

static kefir_result_t scan_storage_class(struct kefir_mem *mem, struct kefir_parser *parser, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));

    struct kefir_source_location source_location = *PARSER_TOKEN_LOCATION(parser, 0);
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
    specifier->source_location = source_location;
    kefir_result_t res = kefir_ast_declarator_specifier_list_append(mem, specifiers, specifier);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_free(mem, specifier);
        return res;
    });

    return KEFIR_OK;
}

static kefir_result_t scan_struct_field_declaration(struct kefir_mem *mem, struct kefir_parser *parser, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct kefir_ast_structure_declaration_entry **, entry_ptr, payload);

    struct kefir_ast_structure_declaration_entry *entry = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE(entry != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST structure declaration entry"));
    kefir_result_t res = kefir_parser_scan_declaration_specifier_list(mem, parser, &entry->declaration.specifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_structure_declaration_entry_free(mem, entry);
        return res;
    });

    kefir_bool_t scan_declarators = !PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_SEMICOLON);
    while (scan_declarators) {
        struct kefir_ast_declarator *declarator = NULL;
        kefir_result_t res = kefir_parser_scan_declarator(mem, parser, &declarator);
        if (res == KEFIR_NO_MATCH) {
            if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COLON)) {
                res = KEFIR_OK;
                declarator = kefir_ast_declarator_identifier(mem, NULL, NULL);
                REQUIRE_CHAIN_SET(
                    &res, declarator != NULL,
                    KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate empty AST declarator identifier"));
                if (declarator != NULL) {
                    declarator->source_location = *PARSER_TOKEN_LOCATION(parser, 0);
                }
            } else {
                res = KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                             "Expected either a declarator or anonymous bit-field");
            }
        }
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_structure_declaration_entry_free(mem, entry);
            return res;
        });

        struct kefir_ast_node_base *bitwidth = NULL;
        if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COLON)) {
            REQUIRE_CHAIN(&res, PARSER_SHIFT(parser));
            REQUIRE_CHAIN(&res, KEFIR_PARSER_RULE_APPLY(mem, parser, constant_expression, &bitwidth));
            if (res == KEFIR_NO_MATCH) {
                res = KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                             "Expected integral constant expression");
            }
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_ast_structure_declaration_entry_free(mem, entry);
                return res;
            });
        }

        res = kefir_ast_structure_declaration_entry_append(mem, entry, declarator, bitwidth);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_structure_declaration_entry_free(mem, entry);
            return res;
        });

        if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COMMA)) {
            res = PARSER_SHIFT(parser);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_ast_structure_declaration_entry_free(mem, entry);
                return res;
            });
            scan_declarators = true;
        } else {
            scan_declarators = false;
        }
    }

    REQUIRE_CHAIN_SET(
        &res, PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_SEMICOLON),
        KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected semicolon"));
    REQUIRE_CHAIN(&res, PARSER_SHIFT(parser));
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_structure_declaration_entry_free(mem, entry);
        return res;
    });
    *entry_ptr = entry;
    return KEFIR_OK;
}

static kefir_result_t scan_struct_static_assert(struct kefir_mem *mem, struct kefir_parser *parser, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct kefir_ast_structure_declaration_entry **, entry, payload);
    struct kefir_ast_node_base *static_assertion = NULL;
    REQUIRE_OK(KEFIR_PARSER_RULE_APPLY(mem, parser, static_assertion, &static_assertion));
    *entry = kefir_ast_structure_declaration_entry_alloc_assert(mem, static_assertion->self);
    REQUIRE_ELSE(*entry != NULL, {
        KEFIR_AST_NODE_FREE(mem, static_assertion);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST declaration static assertion entry");
    });
    return KEFIR_OK;
}

static kefir_result_t scan_struct_specifier_body(struct kefir_mem *mem, struct kefir_parser *parser,
                                                 struct kefir_ast_structure_specifier *specifier) {
    REQUIRE_OK(PARSER_SHIFT(parser));

    kefir_bool_t scan_declarations = true;
    struct kefir_ast_structure_declaration_entry *entry = NULL;
    while (scan_declarations) {
        kefir_result_t res = kefir_parser_try_invoke(mem, parser, scan_struct_field_declaration, &entry);
        if (res == KEFIR_NO_MATCH) {
            res = kefir_parser_try_invoke(mem, parser, scan_struct_static_assert, &entry);
        }

        if (res == KEFIR_NO_MATCH) {
            scan_declarations = false;
        } else {
            REQUIRE_OK(res);
            res = kefir_ast_structure_specifier_append_entry(mem, specifier, entry);
            REQUIRE_ELSE(res == KEFIR_OK, {
                kefir_ast_structure_declaration_entry_free(mem, entry);
                return res;
            });
        }
    }

    REQUIRE(PARSER_TOKEN_IS_RIGHT_BRACE(parser, 0),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                   "Expected field declaration, static assertion or right brace"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    return KEFIR_OK;
}

static kefir_result_t scan_struct_specifier(struct kefir_mem *mem, struct kefir_parser *parser, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_ast_declarator_specifier **, specifier_ptr, payload);

    kefir_bool_t structure = PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_STRUCT);
    REQUIRE_OK(PARSER_SHIFT(parser));
    const char *identifier = NULL;
    kefir_bool_t complete;
    kefir_result_t res = KEFIR_OK;

    if (PARSER_TOKEN_IS_IDENTIFIER(parser, 0)) {
        identifier = kefir_parser_token_cursor_at(parser->cursor, 0)->identifier;
        REQUIRE_OK(PARSER_SHIFT(parser));
        complete = PARSER_TOKEN_IS_LEFT_BRACE(parser, 0);
    } else {
        REQUIRE(PARSER_TOKEN_IS_LEFT_BRACE(parser, 0),
                KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                       "Anonymous structure shall be complete"));
        complete = true;
    }

    struct kefir_ast_structure_specifier *specifier =
        kefir_ast_structure_specifier_init(mem, parser->symbols, identifier, complete);
    REQUIRE(specifier != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST structure specifier"));
    if (complete) {
        res = scan_struct_specifier_body(mem, parser, specifier);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_structure_specifier_free(mem, specifier);
            return res;
        });
    }

    struct kefir_ast_declarator_specifier *decl_specifier = NULL;
    if (structure) {
        decl_specifier = kefir_ast_type_specifier_struct(mem, specifier);
    } else {
        decl_specifier = kefir_ast_type_specifier_union(mem, specifier);
    }
    REQUIRE_ELSE(decl_specifier != NULL, {
        kefir_ast_structure_specifier_free(mem, specifier);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST declarator specifier");
    });

    *specifier_ptr = decl_specifier;

    return KEFIR_OK;
}

static kefir_result_t scan_enum_field_declaration(struct kefir_mem *mem, struct kefir_parser *parser, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct kefir_ast_enum_specifier *, specifier, payload);
    REQUIRE(PARSER_TOKEN_IS_IDENTIFIER(parser, 0),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                   "Expected enumeration constant identifier"));
    const char *identifier = kefir_parser_token_cursor_at(parser->cursor, 0)->identifier;
    REQUIRE_OK(PARSER_SHIFT(parser));

    struct kefir_ast_node_base *value = NULL;
    if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_ASSIGN)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        kefir_result_t res;
        REQUIRE_MATCH_OK(&res, KEFIR_PARSER_RULE_APPLY(mem, parser, constant_expression, &value),
                         KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                                "Expected constant expression"));
    }

    kefir_result_t res = kefir_ast_enum_specifier_append(mem, specifier, parser->symbols, identifier, value);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, value);
        return res;
    });

    REQUIRE_OK(kefir_parser_scope_declare_variable(mem, &parser->scope, identifier));
    return KEFIR_OK;
}

static kefir_result_t scan_enum_specifier_body(struct kefir_mem *mem, struct kefir_parser *parser,
                                               struct kefir_ast_enum_specifier *specifier) {
    REQUIRE_OK(PARSER_SHIFT(parser));

    kefir_bool_t scan_constants = true;
    while (scan_constants) {
        REQUIRE_OK(kefir_parser_try_invoke(mem, parser, scan_enum_field_declaration, specifier));

        if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COMMA)) {
            REQUIRE_OK(PARSER_SHIFT(parser));
            scan_constants = !PARSER_TOKEN_IS_RIGHT_BRACE(parser, 0);
        } else {
            scan_constants = false;
        }
    }

    REQUIRE(PARSER_TOKEN_IS_RIGHT_BRACE(parser, 0),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected right brace"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    return KEFIR_OK;
}

static kefir_result_t scan_enum_specifier(struct kefir_mem *mem, struct kefir_parser *parser, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct kefir_ast_declarator_specifier **, specifier_ptr, payload);

    REQUIRE_OK(PARSER_SHIFT(parser));
    const char *identifier = NULL;
    kefir_bool_t complete;
    kefir_result_t res = KEFIR_OK;

    if (PARSER_TOKEN_IS_IDENTIFIER(parser, 0)) {
        identifier = kefir_parser_token_cursor_at(parser->cursor, 0)->identifier;
        REQUIRE_OK(PARSER_SHIFT(parser));
        complete = PARSER_TOKEN_IS_LEFT_BRACE(parser, 0);
    } else {
        REQUIRE(PARSER_TOKEN_IS_LEFT_BRACE(parser, 0),
                KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                       "Anonymous enumeration shall have complete body"));
        complete = true;
    }

    struct kefir_ast_enum_specifier *specifier =
        kefir_ast_enum_specifier_init(mem, parser->symbols, identifier, complete);
    REQUIRE(specifier != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST enum specifier"));
    if (complete) {
        res = scan_enum_specifier_body(mem, parser, specifier);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_enum_specifier_free(mem, specifier);
            return res;
        });
    }

    struct kefir_ast_declarator_specifier *decl_specifier = kefir_ast_type_specifier_enum(mem, specifier);
    REQUIRE_ELSE(decl_specifier != NULL, {
        kefir_ast_enum_specifier_free(mem, specifier);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST declarator specifier");
    });

    *specifier_ptr = decl_specifier;

    return KEFIR_OK;
}

static kefir_result_t has_type_specifiers(struct kefir_ast_declarator_specifier_list *specifiers,
                                          kefir_bool_t *result) {
    struct kefir_ast_declarator_specifier *specifier = NULL;
    *result = false;
    kefir_result_t res = KEFIR_OK;
    for (struct kefir_list_entry *iter = kefir_ast_declarator_specifier_list_iter(specifiers, &specifier);
         iter != NULL && res == KEFIR_OK && !*result;
         res = kefir_ast_declarator_specifier_list_next(&iter, &specifier)) {

        if (specifier->klass == KEFIR_AST_TYPE_SPECIFIER) {
            *result = true;
        }
    }
    REQUIRE_OK(res);
    return KEFIR_OK;
}

static kefir_result_t scan_type_specifier(struct kefir_mem *mem, struct kefir_parser *parser, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));

    struct kefir_source_location source_location = *PARSER_TOKEN_LOCATION(parser, 0);
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
        specifier = kefir_ast_type_specifier_boolean(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_COMPLEX)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_type_specifier_complex(mem);
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_ATOMIC) &&
               PARSER_TOKEN_IS_PUNCTUATOR(parser, 1, KEFIR_PUNCTUATOR_LEFT_PARENTHESE)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        REQUIRE_OK(PARSER_SHIFT(parser));
        struct kefir_ast_node_base *type_name = NULL;
        kefir_result_t res;
        REQUIRE_MATCH_OK(
            &res, KEFIR_PARSER_RULE_APPLY(mem, parser, type_name, &type_name),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected type name"));
        specifier = kefir_ast_type_specifier_atomic(mem, type_name);
        REQUIRE_ELSE(specifier != NULL, {
            KEFIR_AST_NODE_FREE(mem, type_name);
            return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST declarator specifier");
        });
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_STRUCT)) {
        REQUIRE_OK(kefir_parser_try_invoke(mem, parser, scan_struct_specifier, &specifier));
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_UNION)) {
        REQUIRE_OK(kefir_parser_try_invoke(mem, parser, scan_struct_specifier, &specifier));
    } else if (PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_ENUM)) {
        REQUIRE_OK(kefir_parser_try_invoke(mem, parser, scan_enum_specifier, &specifier));
    } else if (PARSER_TOKEN_IS_IDENTIFIER(parser, 0) &&
               strcmp(PARSER_CURSOR(parser, 0)->identifier, KEFIR_PARSER_BUILTIN_VA_LIST) == 0) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_type_specifier_va_list(mem);
    } else {
        kefir_bool_t has_specs = false;
        REQUIRE_OK(has_type_specifiers(specifiers, &has_specs));
        REQUIRE(PARSER_TOKEN_IS_IDENTIFIER(parser, 0) && !has_specs,
                KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match type specifier"));
        const char *identifier = kefir_parser_token_cursor_at(parser->cursor, 0)->identifier;
        kefir_bool_t is_typedef;
        kefir_result_t res = kefir_parser_scope_is_typedef(&parser->scope, identifier, &is_typedef);
        if (res == KEFIR_NOT_FOUND) {
            return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match type specifier");
        } else {
            REQUIRE_OK(res);
        }
        REQUIRE(is_typedef, KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match type specifier"));
        REQUIRE_OK(PARSER_SHIFT(parser));
        specifier = kefir_ast_type_specifier_typedef(mem, parser->symbols, identifier);
        REQUIRE(specifier != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST declarator specifier"));
    }

    REQUIRE(specifier != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST declarator specifier"));
    specifier->source_location = source_location;
    kefir_result_t res = kefir_ast_declarator_specifier_list_append(mem, specifiers, specifier);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_free(mem, specifier);
        return res;
    });

    return KEFIR_OK;
}

static kefir_result_t scan_type_qualifier(struct kefir_mem *mem, struct kefir_parser *parser, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));

    struct kefir_source_location source_location = *PARSER_TOKEN_LOCATION(parser, 0);
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
    specifier->source_location = source_location;
    kefir_result_t res = kefir_ast_declarator_specifier_list_append(mem, specifiers, specifier);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_free(mem, specifier);
        return res;
    });

    return KEFIR_OK;
}

static kefir_result_t scan_function_specifier(struct kefir_mem *mem, struct kefir_parser *parser, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));

    struct kefir_source_location source_location = *PARSER_TOKEN_LOCATION(parser, 0);
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
    specifier->source_location = source_location;
    kefir_result_t res = kefir_ast_declarator_specifier_list_append(mem, specifiers, specifier);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_free(mem, specifier);
        return res;
    });

    return KEFIR_OK;
}

static kefir_result_t scan_alignment_specifier(struct kefir_mem *mem, struct kefir_parser *parser, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid payload"));

    struct kefir_source_location source_location = *PARSER_TOKEN_LOCATION(parser, 0);
    ASSIGN_DECL_CAST(struct kefir_ast_declarator_specifier_list *, specifiers, payload);
    REQUIRE(PARSER_TOKEN_IS_KEYWORD(parser, 0, KEFIR_KEYWORD_ALIGNAS),
            KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Unable to match alignment specifier"));
    REQUIRE_OK(PARSER_SHIFT(parser));
    REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE),
            KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0), "Expected left parenthese"));
    REQUIRE_OK(PARSER_SHIFT(parser));

    struct kefir_ast_node_base *alignment = NULL;
    kefir_result_t res = KEFIR_PARSER_RULE_APPLY(mem, parser, constant_expression, &alignment);
    if (res == KEFIR_NO_MATCH) {
        res = KEFIR_PARSER_RULE_APPLY(mem, parser, type_name, &alignment);
    }

    if (res == KEFIR_NO_MATCH) {
        return KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                      "Expected either constant expression, or type name");
    } else {
        REQUIRE_OK(res);
    }
    REQUIRE_ELSE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE), {
        KEFIR_AST_NODE_FREE(mem, alignment);
        return KEFIR_SET_SOURCE_ERROR(KEFIR_SYNTAX_ERROR, PARSER_TOKEN_LOCATION(parser, 0),
                                      "Expected right parenthese");
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
    specifier->source_location = source_location;

    res = kefir_ast_declarator_specifier_list_append(mem, specifiers, specifier);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_declarator_specifier_free(mem, specifier);
        return res;
    });

    return KEFIR_OK;
}

kefir_result_t kefir_parser_scan_declaration_specifier(struct kefir_mem *mem, struct kefir_parser *parser,
                                                       struct kefir_ast_declarator_specifier_list *specifiers) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser"));
    REQUIRE(specifiers != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid declarator specifier list"));

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
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid parser"));
    REQUIRE(specifiers != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid declarator specifier list"));

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
