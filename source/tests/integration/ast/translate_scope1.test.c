#include "kefir/ast-translator/local_scope.h"
#include "kefir/test/util.h"
#include "kefir/ir/format.h"
#include <stdio.h>

static kefir_result_t format_global_scope(FILE *out, struct kefir_ast_translator_global_scope *scope) {
    struct kefir_hashtree_node_iterator iter;

    fprintf(out, "external:\n");
    for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(&scope->external_objects, &iter);
        node != NULL;
        node = kefir_hashtree_next(&iter)) {
        ASSIGN_DECL_CAST(const char *, identifier,
            node->key);
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier *, scoped_identifier,
            node->value);
        if (scoped_identifier->external) {
            fprintf(out, "\tdeclare %s: ", identifier);
        } else {
            fprintf(out, "\tdefine %s: ", identifier);
        }
        kefir_ir_format_type(out, scoped_identifier->type);
        fprintf(out, "\n");
    }
    fprintf(out, "\n");

    fprintf(out, "external thread_local:\n");
    for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(&scope->external_thread_local_objects, &iter);
        node != NULL;
        node = kefir_hashtree_next(&iter)) {
        ASSIGN_DECL_CAST(const char *, identifier,
            node->key);
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier *, scoped_identifier,
            node->value);
        if (scoped_identifier->external) {
            fprintf(out, "\tdeclare %s: ", identifier);
        } else {
            fprintf(out, "\tdefine %s: ", identifier);
        }
        kefir_ir_format_type(out, scoped_identifier->type);
        fprintf(out, "\n");
    }
    fprintf(out, "\n");
    
    fprintf(out, "static: ");
    kefir_ir_format_type(out, scope->static_layout);
    fprintf(out, "\n");
    for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(&scope->static_objects, &iter);
        node != NULL;
        node = kefir_hashtree_next(&iter)) {
        ASSIGN_DECL_CAST(const char *, identifier,
            node->key);
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier *, scoped_identifier,
            node->value);
        fprintf(out, "\tdefine %s: static[" KEFIR_SIZE_FMT "]", identifier, scoped_identifier->layout->value);
        fprintf(out, "\n");
    }
    fprintf(out, "\n");

    fprintf(out, "static thread_local: ");
    kefir_ir_format_type(out, scope->static_thread_local_layout);
    fprintf(out, "\n");
    for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(&scope->static_thread_local_objects, &iter);
        node != NULL;
        node = kefir_hashtree_next(&iter)) {
        ASSIGN_DECL_CAST(const char *, identifier,
            node->key);
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier *, scoped_identifier,
            node->value);
        fprintf(out, "\tdefine %s: static thread_local[" KEFIR_SIZE_FMT "]", identifier, scoped_identifier->layout->value);
        fprintf(out, "\n");
    }
    return KEFIR_OK;
}

static kefir_result_t format_local_scope(FILE *out, struct kefir_ast_translator_local_scope *scope) {
    struct kefir_hashtree_node_iterator iter;

    fprintf(out, "local static:\n");
    for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(&scope->static_objects, &iter);
        node != NULL;
        node = kefir_hashtree_next(&iter)) {
        ASSIGN_DECL_CAST(const char *, identifier,
            node->key);
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier *, scoped_identifier,
            node->value);
        fprintf(out, "\tdefine %s: static[" KEFIR_SIZE_FMT "]", identifier, scoped_identifier->layout->value);
        fprintf(out, "\n");
    }
    fprintf(out, "\n");

    fprintf(out, "local static thread_local:\n");
    for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(&scope->static_thread_local_objects, &iter);
        node != NULL;
        node = kefir_hashtree_next(&iter)) {
        ASSIGN_DECL_CAST(const char *, identifier,
            node->key);
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier *, scoped_identifier,
            node->value);
        fprintf(out, "\tdefine %s: static thread_local[" KEFIR_SIZE_FMT "]", identifier, scoped_identifier->layout->value);
        fprintf(out, "\n");
    }
    fprintf(out, "\n");

    fprintf(out, "local: ");
    kefir_ir_format_type(out, scope->local_layout);
    fprintf(out, "\n");
    for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(&scope->local_objects, &iter);
        node != NULL;
        node = kefir_hashtree_next(&iter)) {
        ASSIGN_DECL_CAST(const char *, identifier,
            node->key);
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier *, scoped_identifier,
            node->value);
        fprintf(out, "\tdefine %s: local[" KEFIR_SIZE_FMT "]", identifier, scoped_identifier->layout->value);
        fprintf(out, "\n");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ir_module module;
    struct kefir_ast_translator_environment env;
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));
    REQUIRE_OK(kefir_ast_global_context_init(mem, type_traits,
        &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    REQUIRE_OK(kefir_ast_global_context_declare_external(mem, &global_context,
        "global_var1", kefir_ast_type_signed_int(), NULL));
    REQUIRE_OK(kefir_ast_global_context_define_external(mem, &global_context,
        "global_var2", kefir_ast_type_signed_long(),
        kefir_ast_alignment_as_type(mem, kefir_ast_type_char()), NULL));
    REQUIRE_OK(kefir_ast_global_context_declare_external_thread_local(mem, &global_context,
        "global_var3", kefir_ast_type_float(), NULL));
    REQUIRE_OK(kefir_ast_global_context_define_external_thread_local(mem, &global_context,
        "global_var4", kefir_ast_type_array(mem, context->type_bundle, kefir_ast_type_char(),
            kefir_ast_constant_expression_integer(mem, 16), NULL), NULL, NULL));

    REQUIRE_OK(kefir_ast_global_context_define_static(mem, &global_context,
        "static_var1", kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_unsigned_long()), NULL, NULL));

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(mem, context->type_bundle,
        "", &struct_type1);
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, struct_type1,
        "x", kefir_ast_type_double(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, struct_type1,
        "y", kefir_ast_type_array(mem, context->type_bundle, kefir_ast_type_signed_char(),
            kefir_ast_constant_expression_integer(mem, 32), NULL), NULL));
    REQUIRE_OK(kefir_ast_global_context_define_static(mem, &global_context,
        "static_var2", type1, NULL, NULL));

    REQUIRE_OK(kefir_ast_global_context_define_static_thread_local(mem, &global_context,
        "static_var3", kefir_ast_type_array(mem, context->type_bundle, type1,
            kefir_ast_constant_expression_integer(mem, 1), NULL), NULL, NULL));
    REQUIRE_OK(kefir_ast_global_context_define_static_thread_local(mem, &global_context,
        "static_var4", kefir_ast_type_signed_short(), NULL, NULL));

    REQUIRE_OK(kefir_ast_local_context_declare_external(mem, &local_context,
        "global_var100", kefir_ast_type_unsigned_char(), NULL));
    REQUIRE_OK(kefir_ast_local_context_declare_external_thread_local(mem, &local_context,
        "global_var101", kefir_ast_type_array(mem, context->type_bundle,
            kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_void()),
            kefir_ast_constant_expression_integer(mem, 100), NULL),
            kefir_ast_alignment_const_expression(mem,
                kefir_ast_constant_expression_integer(mem, 32))));

    REQUIRE_OK(kefir_ast_local_context_define_static(mem, &local_context,
        "local_static_var1", kefir_ast_type_array(mem, context->type_bundle,
            kefir_ast_type_char(), kefir_ast_constant_expression_integer(mem, 128), NULL), NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_define_static(mem, &local_context,
        "local_static_var2", kefir_ast_type_signed_long_long(), NULL, NULL));

    REQUIRE_OK(kefir_ast_local_context_define_static_thread_local(mem, &local_context,
        "local_static_var3", kefir_ast_type_qualified(mem, context->type_bundle,
            kefir_ast_type_bool(), (struct kefir_ast_type_qualification){
                .restricted = true
            }),
            kefir_ast_alignment_const_expression(mem, kefir_ast_constant_expression_integer(mem, 8)), NULL));
    REQUIRE_OK(kefir_ast_local_context_define_static_thread_local(mem, &local_context,
        "local_static_var4", kefir_ast_type_float(), NULL, NULL));

    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, &local_context,
        "local_var1", type1, NULL, NULL));
    REQUIRE_OK(kefir_ast_local_context_define_auto(mem, &local_context,
        "local_var2", kefir_ast_type_array(mem, context->type_bundle,
            kefir_ast_type_array(mem, context->type_bundle, kefir_ast_type_signed_int(),
                kefir_ast_constant_expression_integer(mem, 16), NULL),
            kefir_ast_constant_expression_integer(mem, 64), NULL), NULL, NULL));

    struct kefir_ast_translator_global_scope translator_global_scope;
    struct kefir_ast_translator_local_scope translator_local_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_init(mem, &module, &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_local_scope_init(mem, &module, &translator_global_scope, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &module, &global_context, &env, &translator_global_scope));
    REQUIRE_OK(kefir_ast_translate_local_scope(mem, &local_context, &env, &translator_local_scope));
    REQUIRE_OK(format_global_scope(stdout, &translator_global_scope));
    fprintf(stdout, "\n");
    REQUIRE_OK(format_local_scope(stdout, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translator_local_scope_free(mem, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translator_global_scope_free(mem, &translator_global_scope));

    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return KEFIR_OK;
}