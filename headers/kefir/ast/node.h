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

#ifndef KEFIR_AST_NODE_H_
#define KEFIR_AST_NODE_H_

#include "kefir/ast/node_base.h"
#include "kefir/core/symbol_table.h"
#include "kefir/ast/initializer.h"
#include "kefir/ast/declarator.h"

KEFIR_AST_NODE_STRUCT(kefir_ast_constant, {
    kefir_ast_constant_type_t type;
    union {
        kefir_bool_t boolean;
        kefir_int_t character;
        kefir_wchar_t wide_character;
        kefir_char16_t unicode16_character;
        kefir_char32_t unicode32_character;
        kefir_int_t integer;
        kefir_uint_t uinteger;
        kefir_long_t long_integer;
        kefir_ulong_t ulong_integer;
        kefir_long_long_t long_long;
        kefir_ulong_long_t ulong_long;
        kefir_float32_t float32;
        kefir_float64_t float64;
        kefir_long_double_t long_double;
    } value;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_identifier, { const char *identifier; });

KEFIR_AST_NODE_STRUCT(kefir_ast_string_literal, {
    kefir_ast_string_literal_type_t type;
    void *literal;
    kefir_size_t length;
});

typedef struct kefir_ast_type_declaration {
    struct kefir_ast_declarator_specifier_list specifiers;
    struct kefir_ast_declarator *declarator;
} kefir_ast_type_declaration_t;

KEFIR_AST_NODE_STRUCT(kefir_ast_init_declarator, {
    struct kefir_ast_declaration *declaration;
    struct kefir_ast_declarator *declarator;
    struct kefir_ast_initializer *initializer;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_declaration, {
    struct kefir_ast_declarator_specifier_list specifiers;
    struct kefir_list init_declarators;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_type_name, { struct kefir_ast_type_declaration type_decl; });

typedef struct kefir_ast_generic_selection_assoc {
    struct kefir_ast_type_name *type_name;
    struct kefir_ast_node_base *expr;
} kefir_ast_generic_selection_assoc_t;

KEFIR_AST_NODE_STRUCT(kefir_ast_generic_selection, {
    struct kefir_ast_node_base *control;
    struct kefir_list associations;
    struct kefir_ast_node_base *default_assoc;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_compound_literal, {
    struct kefir_ast_type_name *type_name;
    struct kefir_ast_initializer *initializer;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_cast_operator, {
    struct kefir_ast_type_name *type_name;
    struct kefir_ast_node_base *expr;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_array_subscript, {
    struct kefir_ast_node_base *array;
    struct kefir_ast_node_base *subscript;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_function_call, {
    struct kefir_ast_node_base *function;
    struct kefir_list arguments;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_struct_member, {
    struct kefir_ast_node_base *structure;
    const char *member;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_unary_operation, {
    kefir_ast_unary_operation_type_t type;
    struct kefir_ast_node_base *arg;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_binary_operation, {
    kefir_ast_binary_operation_type_t type;
    struct kefir_ast_node_base *arg1;
    struct kefir_ast_node_base *arg2;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_conditional_operator, {
    struct kefir_ast_node_base *condition;
    struct kefir_ast_node_base *expr1;
    struct kefir_ast_node_base *expr2;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_assignment_operator, {
    kefir_ast_assignment_operation_t operation;
    struct kefir_ast_node_base *target;
    struct kefir_ast_node_base *value;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_comma_operator, { struct kefir_list expressions; });

KEFIR_AST_NODE_STRUCT(kefir_ast_static_assertion, {
    struct kefir_ast_node_base *condition;
    struct kefir_ast_string_literal *string;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_labeled_statement, {
    const char *label;
    struct kefir_ast_node_base *statement;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_case_statement, {
    struct kefir_ast_node_base *expression;
    struct kefir_ast_node_base *statement;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_expression_statement, { struct kefir_ast_node_base *expression; });

KEFIR_AST_NODE_STRUCT(kefir_ast_compound_statement, { struct kefir_list block_items; });

KEFIR_AST_NODE_STRUCT(kefir_ast_conditional_statement, {
    struct kefir_ast_node_base *condition;
    struct kefir_ast_node_base *thenBranch;
    struct kefir_ast_node_base *elseBranch;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_switch_statement, {
    struct kefir_ast_node_base *expression;
    struct kefir_ast_node_base *statement;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_while_statement, {
    struct kefir_ast_node_base *controlling_expr;
    struct kefir_ast_node_base *body;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_do_while_statement, {
    struct kefir_ast_node_base *controlling_expr;
    struct kefir_ast_node_base *body;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_for_statement, {
    struct kefir_ast_node_base *init;
    struct kefir_ast_node_base *controlling_expr;
    struct kefir_ast_node_base *tail;
    struct kefir_ast_node_base *body;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_goto_statement, { const char *identifier; });

KEFIR_AST_NODE_STRUCT(kefir_ast_continue_statement, {
    int payload;  // Dummy payload for non-empty struct
});

KEFIR_AST_NODE_STRUCT(kefir_ast_break_statement, {
    int payload;  // Dummy payload for non-empty struct
});

KEFIR_AST_NODE_STRUCT(kefir_ast_return_statement, { struct kefir_ast_node_base *expression; });

KEFIR_AST_NODE_STRUCT(kefir_ast_function_definition, {
    struct kefir_ast_declarator_specifier_list specifiers;
    struct kefir_ast_declarator *declarator;
    struct kefir_list declarations;
    struct kefir_ast_compound_statement *body;
});

KEFIR_AST_NODE_STRUCT(kefir_ast_translation_unit, { struct kefir_list external_definitions; });

KEFIR_AST_NODE_STRUCT(kefir_ast_builtin, {
    kefir_ast_builtin_operator_t builtin;
    struct kefir_list arguments;
});

typedef struct kefir_ast_extension_node kefir_ast_extension_node_t;
typedef struct kefir_ast_extension_node_class {
    kefir_result_t (*free)(struct kefir_mem *, struct kefir_ast_extension_node *);
    kefir_result_t (*clone)(struct kefir_mem *, struct kefir_ast_extension_node *,
                            const struct kefir_ast_extension_node *);
    void *payload;
} kefir_ast_extension_node_class_t;

KEFIR_AST_NODE_STRUCT(kefir_ast_extension_node, {
    const struct kefir_ast_extension_node_class *klass;
    void *payload;
});

struct kefir_ast_constant *kefir_ast_new_constant_bool(struct kefir_mem *, kefir_bool_t);
struct kefir_ast_constant *kefir_ast_new_constant_char(struct kefir_mem *, kefir_int_t);
struct kefir_ast_constant *kefir_ast_new_constant_wide_char(struct kefir_mem *, kefir_wchar_t);
struct kefir_ast_constant *kefir_ast_new_constant_unicode16_char(struct kefir_mem *, kefir_char16_t);
struct kefir_ast_constant *kefir_ast_new_constant_unicode32_char(struct kefir_mem *, kefir_char32_t);
struct kefir_ast_constant *kefir_ast_new_constant_int(struct kefir_mem *, kefir_int_t);
struct kefir_ast_constant *kefir_ast_new_constant_uint(struct kefir_mem *, kefir_uint_t);
struct kefir_ast_constant *kefir_ast_new_constant_long(struct kefir_mem *, kefir_long_t);
struct kefir_ast_constant *kefir_ast_new_constant_ulong(struct kefir_mem *, kefir_ulong_t);
struct kefir_ast_constant *kefir_ast_new_constant_long_long(struct kefir_mem *, kefir_long_long_t);
struct kefir_ast_constant *kefir_ast_new_constant_ulong_long(struct kefir_mem *, kefir_ulong_long_t);
struct kefir_ast_constant *kefir_ast_new_constant_float(struct kefir_mem *, kefir_float32_t);
struct kefir_ast_constant *kefir_ast_new_constant_double(struct kefir_mem *, kefir_float64_t);
struct kefir_ast_constant *kefir_ast_new_constant_long_double(struct kefir_mem *, kefir_long_double_t);
struct kefir_ast_identifier *kefir_ast_new_identifier(struct kefir_mem *, struct kefir_symbol_table *, const char *);

struct kefir_ast_string_literal *kefir_ast_new_string_literal_multibyte(struct kefir_mem *, const char *, kefir_size_t);
struct kefir_ast_string_literal *kefir_ast_new_string_literal_unicode8(struct kefir_mem *, const char *, kefir_size_t);
struct kefir_ast_string_literal *kefir_ast_new_string_literal_unicode16(struct kefir_mem *, const kefir_char16_t *,
                                                                        kefir_size_t);
struct kefir_ast_string_literal *kefir_ast_new_string_literal_unicode32(struct kefir_mem *, const kefir_char32_t *,
                                                                        kefir_size_t);
struct kefir_ast_string_literal *kefir_ast_new_string_literal_wide(struct kefir_mem *, const kefir_wchar_t *,
                                                                   kefir_size_t);

#define KEFIR_AST_MAKE_STRING_LITERAL_MULTIBYTE(_mem, _string) \
    (kefir_ast_new_string_literal_multibyte((_mem), (_string), strlen((_string)) + 1))

struct kefir_ast_generic_selection *kefir_ast_new_generic_selection(struct kefir_mem *, struct kefir_ast_node_base *);

struct kefir_ast_declaration *kefir_ast_new_declaration(struct kefir_mem *);

struct kefir_ast_init_declarator *kefir_ast_new_init_declarator(struct kefir_mem *, struct kefir_ast_declaration *,
                                                                struct kefir_ast_declarator *,
                                                                struct kefir_ast_initializer *);

struct kefir_ast_type_name *kefir_ast_new_type_name(struct kefir_mem *, struct kefir_ast_declarator *);

struct kefir_ast_compound_literal *kefir_ast_new_compound_literal(struct kefir_mem *, struct kefir_ast_type_name *);

struct kefir_ast_cast_operator *kefir_ast_new_cast_operator(struct kefir_mem *, struct kefir_ast_type_name *,
                                                            struct kefir_ast_node_base *);

struct kefir_ast_array_subscript *kefir_ast_new_array_subscript(struct kefir_mem *, struct kefir_ast_node_base *,
                                                                struct kefir_ast_node_base *);

struct kefir_ast_function_call *kefir_ast_new_function_call(struct kefir_mem *, struct kefir_ast_node_base *);

struct kefir_ast_struct_member *kefir_ast_new_struct_member(struct kefir_mem *, struct kefir_symbol_table *,
                                                            struct kefir_ast_node_base *, const char *);
struct kefir_ast_struct_member *kefir_ast_new_struct_indirect_member(struct kefir_mem *, struct kefir_symbol_table *,
                                                                     struct kefir_ast_node_base *, const char *);
struct kefir_ast_unary_operation *kefir_ast_new_unary_operation(struct kefir_mem *, kefir_ast_unary_operation_type_t,
                                                                struct kefir_ast_node_base *);
struct kefir_ast_binary_operation *kefir_ast_new_binary_operation(struct kefir_mem *, kefir_ast_binary_operation_type_t,
                                                                  struct kefir_ast_node_base *,
                                                                  struct kefir_ast_node_base *);

struct kefir_ast_conditional_operator *kefir_ast_new_conditional_operator(struct kefir_mem *,
                                                                          struct kefir_ast_node_base *,
                                                                          struct kefir_ast_node_base *,
                                                                          struct kefir_ast_node_base *);

struct kefir_ast_assignment_operator *kefir_ast_new_simple_assignment(struct kefir_mem *, struct kefir_ast_node_base *,
                                                                      struct kefir_ast_node_base *);

struct kefir_ast_assignment_operator *kefir_ast_new_compound_assignment(struct kefir_mem *,
                                                                        kefir_ast_assignment_operation_t,
                                                                        struct kefir_ast_node_base *,
                                                                        struct kefir_ast_node_base *);

struct kefir_ast_comma_operator *kefir_ast_new_comma_operator(struct kefir_mem *);

struct kefir_ast_static_assertion *kefir_ast_new_static_assertion(struct kefir_mem *, struct kefir_ast_node_base *,
                                                                  struct kefir_ast_string_literal *);

struct kefir_ast_labeled_statement *kefir_ast_new_labeled_statement(struct kefir_mem *, struct kefir_symbol_table *,
                                                                    const char *, struct kefir_ast_node_base *);

struct kefir_ast_case_statement *kefir_ast_new_case_statement(struct kefir_mem *, struct kefir_ast_node_base *,
                                                              struct kefir_ast_node_base *);

struct kefir_ast_expression_statement *kefir_ast_new_expression_statement(struct kefir_mem *,
                                                                          struct kefir_ast_node_base *);

struct kefir_ast_compound_statement *kefir_ast_new_compound_statement(struct kefir_mem *);

struct kefir_ast_conditional_statement *kefir_ast_new_conditional_statement(struct kefir_mem *,
                                                                            struct kefir_ast_node_base *,
                                                                            struct kefir_ast_node_base *,
                                                                            struct kefir_ast_node_base *);

struct kefir_ast_switch_statement *kefir_ast_new_switch_statement(struct kefir_mem *, struct kefir_ast_node_base *,
                                                                  struct kefir_ast_node_base *);

struct kefir_ast_while_statement *kefir_ast_new_while_statement(struct kefir_mem *, struct kefir_ast_node_base *,
                                                                struct kefir_ast_node_base *);

struct kefir_ast_do_while_statement *kefir_ast_new_do_while_statement(struct kefir_mem *, struct kefir_ast_node_base *,
                                                                      struct kefir_ast_node_base *);

struct kefir_ast_for_statement *kefir_ast_new_for_statement(struct kefir_mem *, struct kefir_ast_node_base *,
                                                            struct kefir_ast_node_base *, struct kefir_ast_node_base *,
                                                            struct kefir_ast_node_base *);

struct kefir_ast_goto_statement *kefir_ast_new_goto_statement(struct kefir_mem *, struct kefir_symbol_table *,
                                                              const char *);

struct kefir_ast_continue_statement *kefir_ast_new_continue_statement(struct kefir_mem *);

struct kefir_ast_break_statement *kefir_ast_new_break_statement(struct kefir_mem *);

struct kefir_ast_return_statement *kefir_ast_new_return_statement(struct kefir_mem *, struct kefir_ast_node_base *);

struct kefir_ast_function_definition *kefir_ast_new_function_definition(struct kefir_mem *,
                                                                        struct kefir_ast_declarator *,
                                                                        struct kefir_ast_compound_statement *);

struct kefir_ast_translation_unit *kefir_ast_new_translation_unit(struct kefir_mem *);

struct kefir_ast_builtin *kefir_ast_new_builtin(struct kefir_mem *, kefir_ast_builtin_operator_t);

struct kefir_ast_extension_node *kefir_ast_new_extension_node(struct kefir_mem *,
                                                              const struct kefir_ast_extension_node_class *, void *);

typedef struct kefir_ast_visitor {
    KEFIR_AST_VISITOR_METHOD(generic_handler, kefir_ast_node_base);
    KEFIR_AST_VISITOR_METHOD(constant, kefir_ast_constant);
    KEFIR_AST_VISITOR_METHOD(identifier, kefir_ast_identifier);
    KEFIR_AST_VISITOR_METHOD(string_literal, kefir_ast_string_literal);
    KEFIR_AST_VISITOR_METHOD(generic_selection, kefir_ast_generic_selection);
    KEFIR_AST_VISITOR_METHOD(type_name, kefir_ast_type_name);
    KEFIR_AST_VISITOR_METHOD(init_declarator, kefir_ast_init_declarator);
    KEFIR_AST_VISITOR_METHOD(declaration, kefir_ast_declaration);
    KEFIR_AST_VISITOR_METHOD(cast_operator, kefir_ast_cast_operator);
    KEFIR_AST_VISITOR_METHOD(array_subscript, kefir_ast_array_subscript);
    KEFIR_AST_VISITOR_METHOD(function_call, kefir_ast_function_call);
    KEFIR_AST_VISITOR_METHOD(struct_member, kefir_ast_struct_member);
    KEFIR_AST_VISITOR_METHOD(struct_indirect_member, kefir_ast_struct_member);
    KEFIR_AST_VISITOR_METHOD(unary_operation, kefir_ast_unary_operation);
    KEFIR_AST_VISITOR_METHOD(binary_operation, kefir_ast_binary_operation);
    KEFIR_AST_VISITOR_METHOD(conditional_operator, kefir_ast_conditional_operator);
    KEFIR_AST_VISITOR_METHOD(assignment_operator, kefir_ast_assignment_operator);
    KEFIR_AST_VISITOR_METHOD(comma_operator, kefir_ast_comma_operator);
    KEFIR_AST_VISITOR_METHOD(compound_literal, kefir_ast_compound_literal);
    KEFIR_AST_VISITOR_METHOD(static_assertion, kefir_ast_static_assertion);
    KEFIR_AST_VISITOR_METHOD(labeled_statement, kefir_ast_labeled_statement);
    KEFIR_AST_VISITOR_METHOD(case_statement, kefir_ast_case_statement);
    KEFIR_AST_VISITOR_METHOD(expression_statement, kefir_ast_expression_statement);
    KEFIR_AST_VISITOR_METHOD(compound_statement, kefir_ast_compound_statement);
    KEFIR_AST_VISITOR_METHOD(conditional_statement, kefir_ast_conditional_statement);
    KEFIR_AST_VISITOR_METHOD(switch_statement, kefir_ast_switch_statement);
    KEFIR_AST_VISITOR_METHOD(while_statement, kefir_ast_while_statement);
    KEFIR_AST_VISITOR_METHOD(do_while_statement, kefir_ast_do_while_statement);
    KEFIR_AST_VISITOR_METHOD(for_statement, kefir_ast_for_statement);
    KEFIR_AST_VISITOR_METHOD(goto_statement, kefir_ast_goto_statement);
    KEFIR_AST_VISITOR_METHOD(continue_statement, kefir_ast_continue_statement);
    KEFIR_AST_VISITOR_METHOD(break_statement, kefir_ast_break_statement);
    KEFIR_AST_VISITOR_METHOD(return_statement, kefir_ast_return_statement);
    KEFIR_AST_VISITOR_METHOD(function_definition, kefir_ast_function_definition);
    KEFIR_AST_VISITOR_METHOD(translation_unit, kefir_ast_translation_unit);
    KEFIR_AST_VISITOR_METHOD(builtin, kefir_ast_builtin);
    KEFIR_AST_VISITOR_METHOD(extension_node, kefir_ast_extension_node);
} kefir_ast_visitor_t;

#define KEFIR_AST_NODE_INTERNAL_DEF
#include "kefir/ast/node_helpers.h"
#undef KEFIR_AST_NODE_INTERNAL_DEF

#endif
