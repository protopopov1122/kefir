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

#ifndef KEFIR_AST_DECLARATOR_H_
#define KEFIR_AST_DECLARATOR_H_

#include "kefir/ast/declarator_specifier.h"
#include "kefir/core/source_location.h"

typedef struct kefir_ast_declarator kefir_ast_declarator_t;  // Forward declaration

typedef struct kefir_ast_declarator_pointer {
    struct kefir_ast_type_qualifier_list type_qualifiers;
    struct kefir_ast_declarator *declarator;
} kefir_ast_declarator_pointer_t;

typedef enum kefir_ast_declarator_array_type {
    KEFIR_AST_DECLARATOR_ARRAY_UNBOUNDED,
    KEFIR_AST_DECLARATOR_ARRAY_VLA_UNSPECIFIED,
    KEFIR_AST_DECLARATOR_ARRAY_BOUNDED
} kefir_ast_declarator_array_type_t;

typedef struct kefir_ast_declarator_array {
    struct kefir_ast_type_qualifier_list type_qualifiers;
    kefir_bool_t static_array;
    kefir_ast_declarator_array_type_t type;
    struct kefir_ast_node_base *length;
    struct kefir_ast_declarator *declarator;
} kefir_ast_declarator_array_t;

typedef struct kefir_ast_declarator_function {
    struct kefir_list parameters;
    kefir_bool_t ellipsis;
    struct kefir_ast_declarator *declarator;
} kefir_ast_declarator_function_t;

typedef struct kefir_ast_declarator {
    kefir_ast_declarator_class_t klass;
    union {
        const char *identifier;
        struct kefir_ast_declarator_pointer pointer;
        struct kefir_ast_declarator_array array;
        struct kefir_ast_declarator_function function;
    };

    struct kefir_source_location source_location;
} kefir_ast_declarator_t;

struct kefir_ast_declarator *kefir_ast_declarator_identifier(struct kefir_mem *, struct kefir_symbol_table *,
                                                             const char *);
struct kefir_ast_declarator *kefir_ast_declarator_pointer(struct kefir_mem *, struct kefir_ast_declarator *);
struct kefir_ast_declarator *kefir_ast_declarator_array(struct kefir_mem *, kefir_ast_declarator_array_type_t,
                                                        struct kefir_ast_node_base *, struct kefir_ast_declarator *);
struct kefir_ast_declarator *kefir_ast_declarator_function(struct kefir_mem *, struct kefir_ast_declarator *);
struct kefir_ast_declarator *kefir_ast_declarator_clone(struct kefir_mem *, const struct kefir_ast_declarator *);
kefir_result_t kefir_ast_declarator_free(struct kefir_mem *, struct kefir_ast_declarator *);

kefir_result_t kefir_ast_declarator_is_abstract(struct kefir_ast_declarator *, kefir_bool_t *);
kefir_result_t kefir_ast_declarator_unpack_identifier(struct kefir_ast_declarator *, const char **);

#endif
