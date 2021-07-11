#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast-translator/flow_control.h"
#include "kefir/ast-translator/lvalue.h"
#include "kefir/ast-translator/value.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t translate_function_impl(struct kefir_mem *mem, struct kefir_ast_function_definition *function,
                                              struct kefir_irbuilder_block *builder,
                                              struct kefir_ast_translator_context *context) {
    struct kefir_ast_local_context *local_context =
        function->base.properties.function_definition.scoped_id->function.local_context;
    for (const struct kefir_list_entry *iter =
             kefir_list_tail(&function->base.properties.type->function_type.parameters);
         iter != NULL; iter = iter->prev) {

        ASSIGN_DECL_CAST(struct kefir_ast_function_type_parameter *, param, iter->value);
        const struct kefir_ast_scoped_identifier *scoped_id = NULL;

        if (param->identifier != NULL) {
            REQUIRE_OK(local_context->context.resolve_ordinary_identifier(&local_context->context, param->identifier,
                                                                          &scoped_id));
            REQUIRE_OK(kefir_ast_translator_object_lvalue(mem, context, builder, param->identifier, scoped_id));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_XCHG, 1));
            REQUIRE_OK(kefir_ast_translator_store_value(mem, scoped_id->type, context, builder));
        } else {
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_POP, 0));
        }
    }

    for (const struct kefir_list_entry *iter = kefir_list_head(&function->body->block_items); iter != NULL;
         kefir_list_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, item, iter->value);

        if (item->properties.category == KEFIR_AST_NODE_CATEGORY_STATEMENT) {
            REQUIRE_OK(kefir_ast_translate_statement(mem, item, builder, context));
        } else if (item->properties.category == KEFIR_AST_NODE_CATEGORY_DECLARATION) {
            REQUIRE_OK(kefir_ast_translate_declaration(mem, item, builder, context));
        } else {
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected compound statement item");
        }
    }

    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_function(struct kefir_mem *mem, const struct kefir_ast_node_base *node,
                                            struct kefir_ast_translator_context *context,
                                            struct kefir_ast_translator_global_scope_layout *global_scope_layout) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));
    REQUIRE(global_scope_layout != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator global scope layout"));
    REQUIRE(node->properties.category == KEFIR_AST_NODE_CATEGORY_FUNCTION_DEFINITION &&
                node->klass->type == KEFIR_AST_FUNCTION_DEFINITION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected declaration AST node"));

    ASSIGN_DECL_CAST(struct kefir_ast_function_definition *, function, node->self);

    const struct kefir_ast_translator_resolved_type *function_type = NULL;
    REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_FUNCTION(
        mem, &context->type_cache.resolver, context->environment, context->ast_context->type_bundle,
        context->ast_context->type_traits, context->module,
        function->base.properties.function_definition.scoped_id->type, &function_type));

    const struct kefir_ast_local_context *local_context =
        function->base.properties.function_definition.scoped_id->function.local_context;
    struct kefir_ast_translator_context local_translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init_local(&local_translator_context, &local_context->context, context));

    struct kefir_ast_translator_local_scope_layout local_scope_layout;
    kefir_result_t res =
        kefir_ast_translator_local_scope_layout_init(mem, context->module, global_scope_layout, &local_scope_layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_context_free(mem, &local_translator_context);
        return res;
    });

    res = kefir_ast_translator_build_local_scope_layout(
        mem, local_context, local_translator_context.environment, local_translator_context.module,
        &local_translator_context.type_cache.resolver, &local_scope_layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_local_scope_layout_free(mem, &local_scope_layout);
        kefir_ast_translator_context_free(mem, &local_translator_context);
        return res;
    });

    res = kefir_ast_translator_flow_control_tree_init(mem, local_context->context.flow_control_tree);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_local_scope_layout_free(mem, &local_scope_layout);
        kefir_ast_translator_context_free(mem, &local_translator_context);
        return res;
    });

    struct kefir_ir_function *ir_func =
        kefir_ir_module_new_function(mem, context->module, function_type->function.declaration->ir_function_decl,
                                     local_scope_layout.local_layout, 0);
    REQUIRE_ELSE(ir_func != NULL, {
        kefir_ast_translator_local_scope_layout_free(mem, &local_scope_layout);
        kefir_ast_translator_context_free(mem, &local_translator_context);
        return KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate IR function");
    });

    struct kefir_irbuilder_block builder;
    res = kefir_irbuilder_block_init(mem, &builder, &ir_func->body);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_local_scope_layout_free(mem, &local_scope_layout);
        kefir_ast_translator_context_free(mem, &local_translator_context);
        return res;
    });

    res = translate_function_impl(mem, function, &builder, context);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_IRBUILDER_BLOCK_FREE(&builder);
        kefir_ast_translator_local_scope_layout_free(mem, &local_scope_layout);
        kefir_ast_translator_context_free(mem, &local_translator_context);
        return res;
    });

    res = KEFIR_IRBUILDER_BLOCK_FREE(&builder);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_local_scope_layout_free(mem, &local_scope_layout);
        kefir_ast_translator_context_free(mem, &local_translator_context);
        return res;
    });

    res = kefir_ast_translator_local_scope_layout_free(mem, &local_scope_layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ast_translator_context_free(mem, &local_translator_context);
        return res;
    });

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &local_translator_context));
    return KEFIR_OK;
}