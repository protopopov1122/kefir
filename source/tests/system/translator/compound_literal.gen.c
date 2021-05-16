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

#include "codegen.inc.c"

static kefir_result_t define_compound_literal_function(struct kefir_mem *mem,
                                                     struct function *func,
                                                     struct kefir_ast_context_manager *context_manager,
                                                     const char *name) {
    REQUIRE_OK(kefir_list_init(&func->args));

    struct kefir_ast_struct_type *union_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_union(mem, context_manager->current->type_bundle,
        NULL, &union_type1);
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager->current->symbols, union_type1,
        "fp64", kefir_ast_type_double(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager->current->symbols, union_type1,
        "fp32", kefir_ast_type_array(mem, context_manager->current->type_bundle,
            kefir_ast_type_float(), kefir_ast_constant_expression_integer(mem, 2), NULL), NULL));

    struct kefir_ast_struct_type *struct_type = NULL;
    const struct kefir_ast_type *type = kefir_ast_type_structure(mem, context_manager->current->type_bundle,
        NULL, &struct_type);
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager->current->symbols, struct_type,
        "string", kefir_ast_type_array(mem, context_manager->current->type_bundle,
            kefir_ast_type_qualified(mem, context_manager->current->type_bundle, kefir_ast_type_char(),
                (struct kefir_ast_type_qualification){
                    .constant = true
                }), kefir_ast_constant_expression_integer(mem, 32), NULL), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager->current->symbols, struct_type,
        "length", kefir_ast_type_unsigned_int(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager->current->symbols, struct_type,
        "padding", kefir_ast_type_unsigned_long_long(),
        kefir_ast_alignment_const_expression(mem, kefir_ast_constant_expression_integer(
            mem, 16))));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager->current->symbols, struct_type,
        "ptr", kefir_ast_type_pointer(mem, context_manager->current->type_bundle, kefir_ast_type_void()), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context_manager->current->symbols, struct_type,
        "floats", kefir_ast_type_array(mem, context_manager->current->type_bundle, type1,
            kefir_ast_constant_expression_integer(mem, 4), NULL), NULL));

    struct kefir_ast_function_type *func_type = NULL;
    func->type = kefir_ast_type_function(mem, context_manager->current->type_bundle,
        type, name, &func_type);

    REQUIRE_OK(kefir_ast_global_context_define_function(mem, context_manager->global, KEFIR_AST_FUNCTION_SPECIFIER_NONE,
        func->type));

    REQUIRE_OK(kefir_ast_local_context_init(mem, context_manager->global, &func->local_context));
    REQUIRE_OK(kefir_ast_context_manager_attach_local(&func->local_context, context_manager));


    const char *STRING = "Goodbye, world!";
    struct kefir_ast_compound_literal *compound = kefir_ast_new_compound_literal(mem, type);
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &compound->initializer->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(mem, STRING)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &compound->initializer->list,
        kefir_ast_new_member_desginator(mem, context_manager->current->symbols, "padding", NULL),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1010)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &compound->initializer->list,
        NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(mem, STRING)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &compound->initializer->list,
        kefir_ast_new_member_desginator(mem, context_manager->current->symbols, "length", NULL),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, strlen(STRING))))));

    struct kefir_ast_initializer *list1 = kefir_ast_new_list_initializer(mem);
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &list1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 4.0013)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &list1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 19.88263)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &list1->list,
        kefir_ast_new_index_desginator(mem, 0,
            kefir_ast_new_member_desginator(mem, context_manager->current->symbols, "fp32",
                kefir_ast_new_index_desginator(mem, 2, NULL))),
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 1.02f)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &list1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 2.04f)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &list1->list, NULL,
        kefir_ast_new_expression_initializer(mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 1e5)))));
    REQUIRE_OK(kefir_ast_initializer_list_append(mem, &compound->initializer->list,
        kefir_ast_new_member_desginator(mem, context_manager->current->symbols, "floats", NULL),
        list1));

    func->body = KEFIR_AST_NODE_BASE(compound);
 
    REQUIRE_OK(kefir_ast_context_manager_detach_local(context_manager));
    return KEFIR_OK;
}

static kefir_result_t generate_ir(struct kefir_mem *mem, struct kefir_ir_module *module, struct kefir_ir_target_platform *ir_platform) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, ir_platform));

    struct kefir_ast_context_manager context_manager;
    struct kefir_ast_global_context global_context;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_context_manager_init(&global_context, &context_manager));

    struct function compound1;
    REQUIRE_OK(define_compound_literal_function(mem, &compound1, &context_manager, "compound1"));

    REQUIRE_OK(analyze_function(mem, &compound1, &context_manager));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, &global_context.context, &env, module));
    
    struct kefir_ast_translator_global_scope_layout global_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, module, &global_scope));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, module,
        &global_context, translator_context.environment, &translator_context.type_cache.resolver,
        &global_scope));

    REQUIRE_OK(translate_function(mem, &compound1, &context_manager, &global_scope, &translator_context));

    REQUIRE_OK(kefir_ast_translate_global_scope(mem, module, &global_scope));
    
    REQUIRE_OK(free_function(mem, &compound1));

    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &global_scope));
    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    GENCODE(generate_ir);
    return EXIT_SUCCESS;
}
