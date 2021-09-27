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

#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/module.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/global_scope_layout.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast/context_manager.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/codegen/amd64-sysv.h"
#include "codegen.h"
#include <stdarg.h>

#include "codegen.inc.c"

static kefir_result_t append_specifiers(struct kefir_mem *mem, struct kefir_ast_declarator_specifier_list *list,
                                        int count, ...) {
    va_list args;
    va_start(args, count);
    while (count--) {
        struct kefir_ast_declarator_specifier *specifier = va_arg(args, struct kefir_ast_declarator_specifier *);
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, list, specifier));
    }
    va_end(args);
    return KEFIR_OK;
}

static kefir_result_t define_get_alignof_function(struct kefir_mem *mem, struct function *func,
                                                  struct kefir_ast_context_manager *context_manager,
                                                  struct kefir_ast_declarator_specifier *specifier) {
    func->identifier = "get_alignof";
    REQUIRE_OK(kefir_list_init(&func->args));

    struct kefir_ast_type_name *type_name16 =
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, context_manager->current->symbols, NULL));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name16->type_decl.specifiers, specifier));
    REQUIRE_OK(kefir_ast_analyze_node(mem, context_manager->current, KEFIR_AST_NODE_BASE(type_name16)));

    const struct kefir_ast_type *struct_type = type_name16->base.properties.type;

    struct kefir_ast_function_type *func_type = NULL;
    func->type = kefir_ast_type_function(mem, context_manager->current->type_bundle, struct_type, &func_type);

    REQUIRE_OK(kefir_ast_global_context_define_function(mem, context_manager->global, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                        func->identifier, func->type, NULL, NULL));

    REQUIRE_OK(kefir_ast_local_context_init(mem, context_manager->global, &func->local_context));
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));

    struct kefir_ast_compound_literal *compound_literal = kefir_ast_new_compound_literal(
        mem, (struct kefir_ast_type_name *) KEFIR_AST_NODE_CLONE(mem, KEFIR_AST_NODE_BASE(type_name16))->self);
#define APPEND(_type_name)                                                                           \
    REQUIRE_OK(kefir_ast_initializer_list_append(                                                    \
        mem, &compound_literal->initializer->list, NULL,                                             \
        kefir_ast_new_expression_initializer(                                                        \
            mem, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(mem, KEFIR_AST_OPERATION_ALIGNOF, \
                                                                   KEFIR_AST_NODE_BASE(_type_name))))))

#define MAKE_TYPENAME(_id, _spec_count, ...)                                                                          \
    struct kefir_ast_type_name *_id = kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)); \
    REQUIRE_OK(append_specifiers(mem, &_id->type_decl.specifiers, (_spec_count), __VA_ARGS__));

    MAKE_TYPENAME(type_name1, 1, kefir_ast_type_specifier_bool(mem))
    MAKE_TYPENAME(type_name2, 1, kefir_ast_type_specifier_char(mem))
    MAKE_TYPENAME(type_name3, 2, kefir_ast_type_specifier_unsigned(mem), kefir_ast_type_specifier_char(mem))
    MAKE_TYPENAME(type_name4, 2, kefir_ast_type_specifier_signed(mem), kefir_ast_type_specifier_char(mem))
    MAKE_TYPENAME(type_name5, 2, kefir_ast_type_specifier_unsigned(mem), kefir_ast_type_specifier_short(mem))
    MAKE_TYPENAME(type_name6, 2, kefir_ast_type_specifier_signed(mem), kefir_ast_type_specifier_short(mem))
    MAKE_TYPENAME(type_name7, 2, kefir_ast_type_specifier_unsigned(mem), kefir_ast_type_specifier_int(mem))
    MAKE_TYPENAME(type_name8, 2, kefir_ast_type_specifier_signed(mem), kefir_ast_type_specifier_int(mem))
    MAKE_TYPENAME(type_name9, 2, kefir_ast_type_specifier_unsigned(mem), kefir_ast_type_specifier_long(mem))
    MAKE_TYPENAME(type_name10, 2, kefir_ast_type_specifier_signed(mem), kefir_ast_type_specifier_long(mem))
    MAKE_TYPENAME(type_name11, 3, kefir_ast_type_specifier_unsigned(mem), kefir_ast_type_specifier_long(mem),
                  kefir_ast_type_specifier_long(mem))
    MAKE_TYPENAME(type_name12, 3, kefir_ast_type_specifier_signed(mem), kefir_ast_type_specifier_long(mem),
                  kefir_ast_type_specifier_long(mem))
    MAKE_TYPENAME(type_name13, 1, kefir_ast_type_specifier_float(mem))
    MAKE_TYPENAME(type_name14, 1, kefir_ast_type_specifier_double(mem))
#undef MAKE_TYPENAME

    struct kefir_ast_type_name *type_name15 = kefir_ast_new_type_name(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
    REQUIRE_OK(append_specifiers(mem, &type_name15->type_decl.specifiers, 1, kefir_ast_type_specifier_void(mem)));

    struct kefir_ast_type_name *type_name17 = kefir_ast_new_type_name(
        mem, kefir_ast_declarator_array(mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                                        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 128)),
                                        kefir_ast_declarator_identifier(mem, NULL, NULL)));
    REQUIRE_OK(append_specifiers(mem, &type_name17->type_decl.specifiers, 1, kefir_ast_type_specifier_int(mem)));

    APPEND(type_name1);
    APPEND(type_name2);
    APPEND(type_name3);
    APPEND(type_name4);
    APPEND(type_name5);
    APPEND(type_name6);
    APPEND(type_name7);
    APPEND(type_name8);
    APPEND(type_name9);
    APPEND(type_name10);
    APPEND(type_name11);
    APPEND(type_name12);
    APPEND(type_name13);
    APPEND(type_name14);
    APPEND(type_name15);
    APPEND(type_name16);
    APPEND(type_name17);

    func->body = KEFIR_AST_NODE_BASE(compound_literal);

    REQUIRE_OK(kefir_ast_context_manager_detach_local(context_manager));
    return KEFIR_OK;
}

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module,
                                  struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_context_manager context_manager;
    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_context_manager_init(&global_context, &context_manager));

    struct kefir_ast_structure_specifier *specifier1 =
        kefir_ast_structure_specifier_init(mem, context_manager.current->symbols, NULL, true);
    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry1->declaration.specifiers,
                                                          kefir_ast_type_specifier_int(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "tboolean"), NULL));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "tchar"), NULL));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "tuchar"), NULL));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "tschar"), NULL));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "tushort"), NULL));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "tsshort"), NULL));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "tuint"), NULL));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "tsint"), NULL));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "tulong"), NULL));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "tslong"), NULL));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "tullong"), NULL));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "tsllong"), NULL));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "tfloat"), NULL));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "tdouble"), NULL));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "tpvoid"), NULL));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "tstruct"), NULL));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(
        mem, entry1, kefir_ast_declarator_identifier(mem, context_manager.current->symbols, "tarray"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier1, entry1));

    struct function get_alignof;
    REQUIRE_OK(define_get_alignof_function(mem, &get_alignof, &context_manager,
                                           kefir_ast_type_specifier_struct(mem, specifier1)));

    REQUIRE_OK(analyze_function(mem, &get_alignof, &context_manager));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));

    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module, &global_context,
                                                              translator_context.environment,
                                                              &translator_context.type_cache.resolver, &global_scope));

    REQUIRE_OK(translate_function(mem, &get_alignof, &context_manager, &global_scope, &translator_context));

    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, module, &global_scope));

    REQUIRE_OK(free_function(mem, &get_alignof));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}
