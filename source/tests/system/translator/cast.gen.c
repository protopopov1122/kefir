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
#include "kefir/test/util.h"

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

static kefir_result_t define_cast_function(struct kefir_mem *mem, struct function *func,
                                           struct kefir_ast_context_manager *context_manager, const char *name,
                                           const struct kefir_ast_type *param_type,
                                           struct kefir_ast_type_name *return_type_name) {
    func->identifier = name;
    REQUIRE_OK(kefir_list_init(&func->args));

    REQUIRE_OK(kefir_ast_analyze_node(mem, context_manager->current, KEFIR_AST_NODE_BASE(return_type_name)));

    struct kefir_ast_function_type *func_type = NULL;
    func->type = kefir_ast_type_function(mem, context_manager->current->type_bundle,
                                         return_type_name->base.properties.type, &func_type);
    REQUIRE_OK(
        kefir_ast_type_function_parameter(mem, context_manager->current->type_bundle, func_type, param_type, NULL));

    REQUIRE_OK(kefir_ast_global_context_define_function(mem, context_manager->global, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                                                        name, func->type, NULL, NULL));

    REQUIRE_OK(kefir_ast_local_context_init(mem, context_manager->global, &func->local_context));
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));

    REQUIRE_OK(
        kefir_ast_local_context_define_auto(mem, context_manager->local, "value", param_type, NULL, NULL, NULL, NULL));

    REQUIRE_OK(kefir_list_insert_after(
        mem, &func->args, kefir_list_tail(&func->args),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "value"))));

    func->body = KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(
        mem, return_type_name,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context_manager->current->symbols, "value"))));

    REQUIRE_OK(kefir_ast_context_manager_detach_local(context_manager));
    return KEFIR_OK;
}

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module,
                                  struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_context_manager context_manager;
    struct kefir_ast_global_context global_context;
    REQUIRE_OK(
        kefir_ast_global_context_init(mem, kefir_util_default_type_traits(), &env.target_env, &global_context, NULL));
    REQUIRE_OK(kefir_ast_context_manager_init(&global_context, &context_manager));

    struct kefir_ast_struct_type *struct_type = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_structure(mem, context_manager.current->type_bundle, "", &struct_type);
    REQUIRE_OK(kefir_ast_struct_type_field(
        mem, context_manager.current->symbols, struct_type, "array",
        kefir_ast_type_array(mem, context_manager.current->type_bundle, kefir_ast_type_signed_char(),
                             kefir_ast_constant_expression_integer(mem, 32), NULL),
        NULL));

#define MAKE_TYPENAME(_id, _spec_count, ...)                                                                          \
    struct kefir_ast_type_name *_id = kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)); \
    REQUIRE_OK(append_specifiers(mem, &_id->type_decl.specifiers, (_spec_count), __VA_ARGS__));

    MAKE_TYPENAME(type_name1, 1, kefir_ast_type_specifier_int(mem));
    MAKE_TYPENAME(type_name2, 1, kefir_ast_type_specifier_char(mem));
    MAKE_TYPENAME(type_name3, 1, kefir_ast_type_specifier_int(mem));
    MAKE_TYPENAME(type_name4, 2, kefir_ast_type_specifier_unsigned(mem), kefir_ast_type_specifier_char(mem));
    MAKE_TYPENAME(type_name5, 2, kefir_ast_type_specifier_unsigned(mem), kefir_ast_type_specifier_int(mem));
    MAKE_TYPENAME(type_name6, 2, kefir_ast_type_specifier_unsigned(mem), kefir_ast_type_specifier_char(mem));
    MAKE_TYPENAME(type_name7, 2, kefir_ast_type_specifier_unsigned(mem), kefir_ast_type_specifier_int(mem));
    MAKE_TYPENAME(type_name8, 2, kefir_ast_type_specifier_unsigned(mem), kefir_ast_type_specifier_char(mem));
    MAKE_TYPENAME(type_name9, 1, kefir_ast_type_specifier_float(mem));
    MAKE_TYPENAME(type_name10, 2, kefir_ast_type_specifier_long(mem), kefir_ast_type_specifier_long(mem));
    MAKE_TYPENAME(type_name11, 1, kefir_ast_type_specifier_double(mem));
    MAKE_TYPENAME(type_name12, 1, kefir_ast_type_specifier_short(mem));
    MAKE_TYPENAME(type_name13, 1, kefir_ast_type_specifier_double(mem));
    MAKE_TYPENAME(type_name14, 2, kefir_ast_type_specifier_unsigned(mem), kefir_ast_type_specifier_int(mem));
    MAKE_TYPENAME(type_name15, 1, kefir_ast_type_specifier_double(mem));
    MAKE_TYPENAME(type_name16, 1, kefir_ast_type_specifier_float(mem));
    MAKE_TYPENAME(type_name17, 3, kefir_ast_type_specifier_unsigned(mem), kefir_ast_type_specifier_long(mem),
                  kefir_ast_type_specifier_long(mem));
    MAKE_TYPENAME(type_name18, 1, kefir_ast_type_specifier_void(mem));
    MAKE_TYPENAME(type_name19, 1, kefir_ast_type_specifier_void(mem));
    MAKE_TYPENAME(type_name20, 1, kefir_ast_type_specifier_void(mem));
    MAKE_TYPENAME(type_name21, 1, kefir_ast_type_specifier_void(mem));
#undef MAKE_TYPENAME

    struct kefir_ast_type_name *type_name22 = kefir_ast_new_type_name(
        mem, kefir_ast_declarator_pointer(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &type_name22->type_decl.specifiers,
                                                          kefir_ast_type_specifier_void(mem)));

    struct function char_int_cast, int_char_cast, uchar_int_cast, int_uchar_cast, char_uint_cast, uint_char_cast,
        uchar_uint_cast, uint_uchar_cast, long_float_cast, float_long_cast, short_double_cast, double_short_cast,
        uint_double_cast, double_uint_cast, float_double_cast, double_float_cast, ulong_voidptr_cast,
        voidptr_ulong_cast, int_void_cast, double_void_cast, voidptr_void_cast, struct_void_cast;
    REQUIRE_OK(define_cast_function(mem, &char_int_cast, &context_manager, "char_int_cast", kefir_ast_type_char(),
                                    type_name1));
    REQUIRE_OK(define_cast_function(mem, &int_char_cast, &context_manager, "int_char_cast", kefir_ast_type_signed_int(),
                                    type_name2));
    REQUIRE_OK(define_cast_function(mem, &uchar_int_cast, &context_manager, "uchar_int_cast",
                                    kefir_ast_type_unsigned_char(), type_name3));
    REQUIRE_OK(define_cast_function(mem, &int_uchar_cast, &context_manager, "int_uchar_cast",
                                    kefir_ast_type_signed_int(), type_name4));
    REQUIRE_OK(define_cast_function(mem, &char_uint_cast, &context_manager, "char_uint_cast", kefir_ast_type_char(),
                                    type_name5));
    REQUIRE_OK(define_cast_function(mem, &uint_char_cast, &context_manager, "uint_char_cast",
                                    kefir_ast_type_signed_int(), type_name6));
    REQUIRE_OK(define_cast_function(mem, &uchar_uint_cast, &context_manager, "uchar_uint_cast",
                                    kefir_ast_type_unsigned_char(), type_name7));
    REQUIRE_OK(define_cast_function(mem, &uint_uchar_cast, &context_manager, "uint_uchar_cast",
                                    kefir_ast_type_unsigned_int(), type_name8));
    REQUIRE_OK(define_cast_function(mem, &long_float_cast, &context_manager, "long_float_cast",
                                    kefir_ast_type_signed_long(), type_name9));
    REQUIRE_OK(define_cast_function(mem, &float_long_cast, &context_manager, "float_long_cast", kefir_ast_type_float(),
                                    type_name10));
    REQUIRE_OK(define_cast_function(mem, &short_double_cast, &context_manager, "short_double_cast",
                                    kefir_ast_type_signed_short(), type_name11));
    REQUIRE_OK(define_cast_function(mem, &double_short_cast, &context_manager, "double_short_cast",
                                    kefir_ast_type_double(), type_name12));
    REQUIRE_OK(define_cast_function(mem, &uint_double_cast, &context_manager, "uint_double_cast",
                                    kefir_ast_type_unsigned_int(), type_name13));
    REQUIRE_OK(define_cast_function(mem, &double_uint_cast, &context_manager, "double_uint_cast",
                                    kefir_ast_type_double(), type_name14));
    REQUIRE_OK(define_cast_function(mem, &float_double_cast, &context_manager, "float_double_cast",
                                    kefir_ast_type_float(), type_name15));
    REQUIRE_OK(define_cast_function(mem, &double_float_cast, &context_manager, "double_float_cast",
                                    kefir_ast_type_double(), type_name16));
    REQUIRE_OK(define_cast_function(mem, &ulong_voidptr_cast, &context_manager, "ulong_voidptr_cast",
                                    kefir_ast_type_unsigned_long(), type_name22));
    REQUIRE_OK(define_cast_function(
        mem, &voidptr_ulong_cast, &context_manager, "voidptr_ulong_cast",
        kefir_ast_type_pointer(mem, context_manager.current->type_bundle, kefir_ast_type_void()), type_name17));
    REQUIRE_OK(define_cast_function(mem, &int_void_cast, &context_manager, "int_void_cast", kefir_ast_type_signed_int(),
                                    type_name18));
    REQUIRE_OK(define_cast_function(mem, &double_void_cast, &context_manager, "double_void_cast",
                                    kefir_ast_type_double(), type_name19));
    REQUIRE_OK(define_cast_function(
        mem, &voidptr_void_cast, &context_manager, "voidptr_void_cast",
        kefir_ast_type_pointer(mem, context_manager.current->type_bundle, kefir_ast_type_void()), type_name20));
    REQUIRE_OK(define_cast_function(mem, &struct_void_cast, &context_manager, "struct_void_cast", type1, type_name21));

    REQUIRE_OK(analyze_function(mem, &char_int_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &int_char_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &uchar_int_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &int_uchar_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &char_uint_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &uint_char_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &uchar_uint_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &uint_uchar_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &long_float_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &float_long_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &short_double_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &double_short_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &uint_double_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &double_uint_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &float_double_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &double_float_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &ulong_voidptr_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &voidptr_ulong_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &int_void_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &double_void_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &voidptr_void_cast, &context_manager));
    REQUIRE_OK(analyze_function(mem, &struct_void_cast, &context_manager));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(
        kefir_ast_translator_context_init(mem, &translator_context, &global_context.context, &env, module, NULL));

    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module, &global_context,
                                                              translator_context.environment,
                                                              &translator_context.type_cache.resolver, &global_scope));

    REQUIRE_OK(translate_function(mem, &char_int_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &int_char_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &uchar_int_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &int_uchar_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &char_uint_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &uint_char_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &uchar_uint_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &uint_uchar_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &long_float_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &float_long_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &short_double_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &double_short_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &uint_double_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &double_uint_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &float_double_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &double_float_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &ulong_voidptr_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &voidptr_ulong_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &int_void_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &double_void_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &voidptr_void_cast, &context_manager, &global_scope, &translator_context));
    REQUIRE_OK(translate_function(mem, &struct_void_cast, &context_manager, &global_scope, &translator_context));

    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &global_context.context, module, &global_scope));

    REQUIRE_OK(free_function(mem, &char_int_cast));
    REQUIRE_OK(free_function(mem, &int_char_cast));
    REQUIRE_OK(free_function(mem, &uchar_int_cast));
    REQUIRE_OK(free_function(mem, &int_uchar_cast));
    REQUIRE_OK(free_function(mem, &char_uint_cast));
    REQUIRE_OK(free_function(mem, &uint_char_cast));
    REQUIRE_OK(free_function(mem, &uchar_uint_cast));
    REQUIRE_OK(free_function(mem, &uint_uchar_cast));
    REQUIRE_OK(free_function(mem, &long_float_cast));
    REQUIRE_OK(free_function(mem, &float_long_cast));
    REQUIRE_OK(free_function(mem, &short_double_cast));
    REQUIRE_OK(free_function(mem, &double_short_cast));
    REQUIRE_OK(free_function(mem, &uint_double_cast));
    REQUIRE_OK(free_function(mem, &double_uint_cast));
    REQUIRE_OK(free_function(mem, &float_double_cast));
    REQUIRE_OK(free_function(mem, &double_float_cast));
    REQUIRE_OK(free_function(mem, &ulong_voidptr_cast));
    REQUIRE_OK(free_function(mem, &voidptr_ulong_cast));
    REQUIRE_OK(free_function(mem, &int_void_cast));
    REQUIRE_OK(free_function(mem, &double_void_cast));
    REQUIRE_OK(free_function(mem, &voidptr_void_cast));
    REQUIRE_OK(free_function(mem, &struct_void_cast));

    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}
