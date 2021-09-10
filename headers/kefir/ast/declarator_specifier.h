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

#ifndef KEFIR_AST_DECLARATOR_SPECIFIER_H_
#define KEFIR_AST_DECLARATOR_SPECIFIER_H_

#include "kefir/core/mem.h"
#include "kefir/core/symbol_table.h"
#include "kefir/core/list.h"
#include "kefir/ast/constants.h"
#include "kefir/ast/base.h"
#include "kefir/core/source_location.h"

typedef struct kefir_ast_declarator_specifier kefir_ast_declarator_specifier_t;
typedef struct kefir_ast_declarator kefir_ast_declarator_t;

typedef struct kefir_ast_declarator_specifier_list {
    struct kefir_list list;
} kefir_ast_declarator_specifier_list_t;

kefir_result_t kefir_ast_declarator_specifier_list_init(struct kefir_ast_declarator_specifier_list *);
kefir_result_t kefir_ast_declarator_specifier_list_free(struct kefir_mem *,
                                                        struct kefir_ast_declarator_specifier_list *);
kefir_result_t kefir_ast_declarator_specifier_list_append(struct kefir_mem *,
                                                          struct kefir_ast_declarator_specifier_list *,
                                                          struct kefir_ast_declarator_specifier *);
struct kefir_list_entry *kefir_ast_declarator_specifier_list_iter(const struct kefir_ast_declarator_specifier_list *,
                                                                  struct kefir_ast_declarator_specifier **);
kefir_result_t kefir_ast_declarator_specifier_list_next(struct kefir_list_entry **,
                                                        struct kefir_ast_declarator_specifier **);
kefir_result_t kefir_ast_declarator_specifier_list_remove(struct kefir_mem *,
                                                          struct kefir_ast_declarator_specifier_list *,
                                                          struct kefir_list_entry *);
kefir_result_t kefir_ast_declarator_specifier_list_clone(struct kefir_mem *,
                                                         struct kefir_ast_declarator_specifier_list *,
                                                         const struct kefir_ast_declarator_specifier_list *);
kefir_result_t kefir_ast_declarator_specifier_list_move_all(struct kefir_ast_declarator_specifier_list *,
                                                            struct kefir_ast_declarator_specifier_list *);
const struct kefir_source_location *kefir_ast_declarator_specifier_list_source_location(
    const struct kefir_ast_declarator_specifier_list *);

typedef struct kefir_ast_structure_entry_declarator {
    struct kefir_ast_declarator *declarator;
    struct kefir_ast_node_base *bitwidth;
} kefir_ast_structure_entry_declarator;

typedef struct kefir_ast_structure_declaration_entry {
    kefir_bool_t is_static_assertion;
    union {
        struct kefir_ast_static_assertion *static_assertion;
        struct {
            struct kefir_ast_declarator_specifier_list specifiers;
            struct kefir_list declarators;
        } declaration;
    };
} kefir_ast_structure_declaration_entry_t;

typedef struct kefir_ast_structure_specifier {
    const char *identifier;
    kefir_bool_t complete;
    struct kefir_list entries;
} kefir_ast_structure_specifier_t;

struct kefir_ast_structure_specifier *kefir_ast_structure_specifier_init(struct kefir_mem *,
                                                                         struct kefir_symbol_table *, const char *,
                                                                         kefir_bool_t);

kefir_result_t kefir_ast_structure_specifier_free(struct kefir_mem *, struct kefir_ast_structure_specifier *);

struct kefir_ast_structure_specifier *kefir_ast_structure_specifier_clone(struct kefir_mem *,
                                                                          const struct kefir_ast_structure_specifier *);

kefir_result_t kefir_ast_structure_specifier_append_entry(struct kefir_mem *, struct kefir_ast_structure_specifier *,
                                                          struct kefir_ast_structure_declaration_entry *);

struct kefir_ast_structure_declaration_entry *kefir_ast_structure_declaration_entry_alloc(struct kefir_mem *);

struct kefir_ast_structure_declaration_entry *kefir_ast_structure_declaration_entry_alloc_assert(
    struct kefir_mem *, struct kefir_ast_static_assertion *);

kefir_result_t kefir_ast_structure_declaration_entry_free(struct kefir_mem *,
                                                          struct kefir_ast_structure_declaration_entry *);

kefir_result_t kefir_ast_structure_declaration_entry_append(struct kefir_mem *,
                                                            struct kefir_ast_structure_declaration_entry *,
                                                            struct kefir_ast_declarator *,
                                                            struct kefir_ast_node_base *);

typedef struct kefir_ast_enum_specifier_entry {
    const char *constant;
    struct kefir_ast_node_base *value;
} kefir_ast_enum_specifier_entry_t;

typedef struct kefir_ast_enum_specifier {
    const char *identifier;
    kefir_bool_t complete;
    struct kefir_list entries;
} kefir_ast_enum_specifier_t;

struct kefir_ast_enum_specifier *kefir_ast_enum_specifier_init(struct kefir_mem *, struct kefir_symbol_table *,
                                                               const char *, kefir_bool_t);

kefir_result_t kefir_ast_enum_specifier_free(struct kefir_mem *, struct kefir_ast_enum_specifier *);

struct kefir_ast_enum_specifier *kefir_ast_enum_specifier_clone(struct kefir_mem *,
                                                                const struct kefir_ast_enum_specifier *);

kefir_result_t kefir_ast_enum_specifier_append(struct kefir_mem *, struct kefir_ast_enum_specifier *,
                                               struct kefir_symbol_table *, const char *, struct kefir_ast_node_base *);

typedef struct kefir_ast_type_specifier {
    kefir_ast_type_specifier_type_t specifier;
    union {
        struct kefir_ast_node_base *atomic_type;
        struct kefir_ast_structure_specifier *structure;
        struct kefir_ast_enum_specifier *enumeration;
        const char *type_name;
    } value;
} kefir_ast_type_specifier_t;

typedef struct kefir_ast_declarator_specifier {
    kefir_ast_declarator_specifier_class_t klass;
    union {
        struct kefir_ast_type_specifier type_specifier;
        kefir_ast_storage_class_specifier_type_t storage_class;
        kefir_ast_type_qualifier_type_t type_qualifier;
        kefir_ast_function_specifier_type_t function_specifier;
        struct kefir_ast_node_base *alignment_specifier;
    };
    struct kefir_source_location source_location;
} kefir_ast_declarator_specifier_t;

struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_void(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_char(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_short(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_int(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_long(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_float(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_double(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_signed(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_unsigned(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_bool(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_complex(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_atomic(struct kefir_mem *,
                                                                       struct kefir_ast_node_base *);
struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_struct(struct kefir_mem *,
                                                                       struct kefir_ast_structure_specifier *);
struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_union(struct kefir_mem *,
                                                                      struct kefir_ast_structure_specifier *);
struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_enum(struct kefir_mem *,
                                                                     struct kefir_ast_enum_specifier *);
struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_typedef(struct kefir_mem *, struct kefir_symbol_table *,
                                                                        const char *);

struct kefir_ast_declarator_specifier *kefir_ast_storage_class_specifier_typedef(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_storage_class_specifier_extern(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_storage_class_specifier_static(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_storage_class_specifier_thread_local(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_storage_class_specifier_auto(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_storage_class_specifier_register(struct kefir_mem *);

struct kefir_ast_declarator_specifier *kefir_ast_type_qualifier_const(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_type_qualifier_restrict(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_type_qualifier_volatile(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_type_qualifier_atomic(struct kefir_mem *);

struct kefir_ast_declarator_specifier *kefir_ast_function_specifier_inline(struct kefir_mem *);
struct kefir_ast_declarator_specifier *kefir_ast_function_specifier_noreturn(struct kefir_mem *);

struct kefir_ast_declarator_specifier *kefir_ast_alignment_specifier(struct kefir_mem *, struct kefir_ast_node_base *);

struct kefir_ast_declarator_specifier *kefir_ast_declarator_specifier_clone(
    struct kefir_mem *, const struct kefir_ast_declarator_specifier *);
kefir_result_t kefir_ast_declarator_specifier_free(struct kefir_mem *, struct kefir_ast_declarator_specifier *);

typedef struct kefir_ast_type_qualifier_list {
    struct kefir_list list;
} kefir_ast_type_qualifier_list_t;

kefir_result_t kefir_ast_type_qualifier_list_init(struct kefir_ast_type_qualifier_list *);
kefir_result_t kefir_ast_type_qualifier_list_free(struct kefir_mem *, struct kefir_ast_type_qualifier_list *);
kefir_result_t kefir_ast_type_qualifier_list_append(struct kefir_mem *, struct kefir_ast_type_qualifier_list *,
                                                    kefir_ast_type_qualifier_type_t);
struct kefir_list_entry *kefir_ast_type_qualifier_list_iter(const struct kefir_ast_type_qualifier_list *,
                                                            kefir_ast_type_qualifier_type_t *);
kefir_result_t kefir_ast_type_qualifier_list_next(const struct kefir_list_entry **, kefir_ast_type_qualifier_type_t *);
kefir_result_t kefir_ast_type_qualifier_list_remove(struct kefir_mem *, struct kefir_ast_type_qualifier_list *,
                                                    struct kefir_list_entry *);
kefir_result_t kefir_ast_type_qualifier_list_clone(struct kefir_mem *, struct kefir_ast_type_qualifier_list *,
                                                   const struct kefir_ast_type_qualifier_list *);

#endif
