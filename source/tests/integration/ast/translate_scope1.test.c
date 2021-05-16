#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast-translator/type_cache.h"
#include "kefir/test/util.h"
#include "kefir/ir/format.h"
#include <stdio.h>

static kefir_result_t format_global_scope(struct kefir_json_output *json, struct kefir_ast_translator_global_scope_layout *scope) {
    REQUIRE_OK(kefir_json_output_object_key(json, "externals"));
    REQUIRE_OK(kefir_json_output_array_begin(json));
    for (const struct kefir_list_entry *iter = kefir_list_head(&scope->external_objects);
        iter != NULL;
        kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_entry *, scoped_identifier,
            iter->value);
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, scoped_identifier_payload,
            scoped_identifier->value->payload.ptr);

        REQUIRE_OK(kefir_json_output_object_begin(json));
        REQUIRE_OK(kefir_json_output_object_key(json, "class"));
        if (scoped_identifier->value->object.external) {
            REQUIRE_OK(kefir_json_output_string(json, "declaration"));
        } else {
            REQUIRE_OK(kefir_json_output_string(json, "definition"));
        }
        REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
        REQUIRE_OK(kefir_json_output_string(json, scoped_identifier->identifier));
        REQUIRE_OK(kefir_json_output_object_key(json, "type"));
        REQUIRE_OK(kefir_ir_format_type_json(json, scoped_identifier_payload->type));
        REQUIRE_OK(kefir_json_output_object_end(json));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));

    REQUIRE_OK(kefir_json_output_object_key(json, "external_thread_locals"));
    REQUIRE_OK(kefir_json_output_array_begin(json));
    for (const struct kefir_list_entry *iter = kefir_list_head(&scope->external_thread_local_objects);
        iter != NULL;
        kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_entry *, scoped_identifier,
            iter->value);
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, scoped_identifier_payload,
            scoped_identifier->value->payload.ptr);
        
        REQUIRE_OK(kefir_json_output_object_begin(json));
        REQUIRE_OK(kefir_json_output_object_key(json, "class"));
        if (scoped_identifier->value->object.external) {
            REQUIRE_OK(kefir_json_output_string(json, "declaration"));
        } else {
            REQUIRE_OK(kefir_json_output_string(json, "definition"));
        }
        REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
        REQUIRE_OK(kefir_json_output_string(json, scoped_identifier->identifier));
        REQUIRE_OK(kefir_json_output_object_key(json, "type"));
        REQUIRE_OK(kefir_ir_format_type_json(json, scoped_identifier_payload->type));
        REQUIRE_OK(kefir_json_output_object_end(json));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    
    REQUIRE_OK(kefir_json_output_object_key(json, "statics"));
    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "layout"));
    REQUIRE_OK(kefir_ir_format_type_json(json, scope->static_layout));
    REQUIRE_OK(kefir_json_output_object_key(json, "variables"));
    REQUIRE_OK(kefir_json_output_array_begin(json));
    for (const struct kefir_list_entry *iter = kefir_list_head(&scope->static_objects);
        iter != NULL;
        kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_entry *, scoped_identifier,
            iter->value);
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, scoped_identifier_payload,
            scoped_identifier->value->payload.ptr);

        REQUIRE_OK(kefir_json_output_object_begin(json));
        REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
        REQUIRE_OK(kefir_json_output_string(json, scoped_identifier->identifier));
        REQUIRE_OK(kefir_json_output_object_key(json, "offset"));
        REQUIRE_OK(kefir_json_output_uinteger(json, scoped_identifier_payload->layout->value));
        REQUIRE_OK(kefir_json_output_object_end(json));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    REQUIRE_OK(kefir_json_output_object_end(json));

    REQUIRE_OK(kefir_json_output_object_key(json, "static_thread_locals"));
    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "layout"));
    REQUIRE_OK(kefir_ir_format_type_json(json, scope->static_thread_local_layout));
    REQUIRE_OK(kefir_json_output_object_key(json, "variables"));
    REQUIRE_OK(kefir_json_output_array_begin(json));
    for (const struct kefir_list_entry *iter = kefir_list_head(&scope->static_thread_local_objects);
        iter != NULL;
        kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_entry *, scoped_identifier,
            iter->value);
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, scoped_identifier_payload,
            scoped_identifier->value->payload.ptr);

        REQUIRE_OK(kefir_json_output_object_begin(json));
        REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
        REQUIRE_OK(kefir_json_output_string(json, scoped_identifier->identifier));
        REQUIRE_OK(kefir_json_output_object_key(json, "offset"));
        REQUIRE_OK(kefir_json_output_uinteger(json, scoped_identifier_payload->layout->value));
        REQUIRE_OK(kefir_json_output_object_end(json));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t format_local_scope(struct kefir_json_output *json, struct kefir_ast_translator_local_scope_layout *scope) {
    
    REQUIRE_OK(kefir_json_output_object_key(json, "local_statics"));
    REQUIRE_OK(kefir_json_output_array_begin(json));
    for (const struct kefir_list_entry *iter = kefir_list_head(&scope->static_objects);
        iter != NULL;
        kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_entry *, scoped_identifier,
            iter->value);
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, scoped_identifier_payload,
            scoped_identifier->value->payload.ptr);
        
        REQUIRE_OK(kefir_json_output_object_begin(json));
        REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
        REQUIRE_OK(kefir_json_output_string(json, scoped_identifier->identifier));
        REQUIRE_OK(kefir_json_output_object_key(json, "offset"));
        REQUIRE_OK(kefir_json_output_uinteger(json, scoped_identifier_payload->layout->value));
        REQUIRE_OK(kefir_json_output_object_end(json));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));

    REQUIRE_OK(kefir_json_output_object_key(json, "local_thread_local_statics"));
    REQUIRE_OK(kefir_json_output_array_begin(json));
    for (const struct kefir_list_entry *iter = kefir_list_head(&scope->static_thread_local_objects);
        iter != NULL;
        kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_entry *, scoped_identifier,
            iter->value);
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, scoped_identifier_payload,
            scoped_identifier->value->payload.ptr);
        
        REQUIRE_OK(kefir_json_output_object_begin(json));
        REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
        REQUIRE_OK(kefir_json_output_string(json, scoped_identifier->identifier));
        REQUIRE_OK(kefir_json_output_object_key(json, "offset"));
        REQUIRE_OK(kefir_json_output_uinteger(json, scoped_identifier_payload->layout->value));
        REQUIRE_OK(kefir_json_output_object_end(json));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));

    REQUIRE_OK(kefir_json_output_object_key(json, "locals"));
    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "layout"));
    REQUIRE_OK(kefir_ir_format_type_json(json, scope->local_layout));
    REQUIRE_OK(kefir_json_output_object_key(json, "variables"));
    REQUIRE_OK(kefir_json_output_array_begin(json));
    for (const struct kefir_list_entry *iter = kefir_list_head(&scope->local_objects);
        iter != NULL;
        kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_entry *, scoped_identifier,
            iter->value);
        ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, scoped_identifier_payload,
            scoped_identifier->value->payload.ptr);
        
        REQUIRE_OK(kefir_json_output_object_begin(json));
        REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
        REQUIRE_OK(kefir_json_output_string(json, scoped_identifier->identifier));
        REQUIRE_OK(kefir_json_output_object_key(json, "offset"));
        REQUIRE_OK(kefir_json_output_uinteger(json, scoped_identifier_payload->layout->value));
        REQUIRE_OK(kefir_json_output_object_end(json));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    REQUIRE_OK(kefir_json_output_object_end(json));
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

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    struct kefir_ast_translator_local_scope_layout translator_local_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_init(mem, &module, &translator_global_scope, &translator_local_scope));

    struct kefir_ast_translator_type_cache type_cache;
    REQUIRE_OK(kefir_ast_translator_type_cache_init(&type_cache, NULL));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, &module, &global_context, &env,
        &type_cache.resolver, &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(mem, &local_context, &env, &module,
        &type_cache.resolver, &translator_local_scope));

    struct kefir_json_output json;
    REQUIRE_OK(kefir_json_output_init(&json, stdout, 4));
    REQUIRE_OK(kefir_json_output_object_begin(&json));
    REQUIRE_OK(format_global_scope(&json, &translator_global_scope));
    REQUIRE_OK(format_local_scope(&json, &translator_local_scope));
    REQUIRE_OK(kefir_json_output_object_end(&json));
    REQUIRE_OK(kefir_json_output_finalize(&json));

    REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_FREE(mem, &type_cache.resolver));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));

    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return KEFIR_OK;
}
