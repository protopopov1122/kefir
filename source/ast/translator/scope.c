#include "kefir/ast/translator/scope.h"
#include "kefir/ast/translator/translator.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_global_scope_layout_init(struct kefir_mem *mem,
                                              struct kefir_ir_module *module,
                                              struct kefir_ast_global_scope_layout *layout) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module"));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST global scope layout"));
    REQUIRE_OK(kefir_hashtree_init(&layout->external_objects, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_hashtree_init(&layout->external_thread_local_objects, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_hashtree_init(&layout->static_objects, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_hashtree_init(&layout->static_thread_local_objects, &kefir_hashtree_str_ops));
    layout->static_layout = kefir_ir_module_new_type(mem, module, 0, NULL);
    layout->static_thread_local_layout = kefir_ir_module_new_type(mem, module, 0, NULL);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_local_scope_layout_init(struct kefir_mem *mem,
                                             struct kefir_ir_module *module,
                                             struct kefir_ast_global_scope_layout *global,
                                             struct kefir_ast_local_scope_layout *layout) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module"));
    REQUIRE(global != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST global scope layout"));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST local scope layout"));
    layout->global = global;
    layout->locals = kefir_ir_module_new_type(mem, module, 0, NULL);
    REQUIRE_OK(kefir_hashtree_init(&layout->static_locals, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_hashtree_init(&layout->static_thread_local_locals, &kefir_hashtree_str_ops));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_scope_layout_free(struct kefir_mem *mem,
                                              struct kefir_ast_global_scope_layout *layout) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST global scope layout"));
    REQUIRE_OK(kefir_hashtree_free(mem, &layout->external_objects));
    REQUIRE_OK(kefir_hashtree_free(mem, &layout->external_thread_local_objects));
    REQUIRE_OK(kefir_hashtree_free(mem, &layout->static_objects));
    REQUIRE_OK(kefir_hashtree_free(mem, &layout->static_thread_local_objects));
    layout->static_layout = NULL;
    layout->static_thread_local_layout = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_lobal_scope_layout_free(struct kefir_mem *mem,
                                             struct kefir_ast_local_scope_layout *layout) {
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST local scope layout"));
    REQUIRE_OK(kefir_hashtree_free(mem, &layout->static_thread_local_locals));
    REQUIRE_OK(kefir_hashtree_free(mem, &layout->static_locals));
    layout->global = NULL;
    layout->locals = NULL;
    return KEFIR_OK;
}

static kefir_result_t translate_global_scoped_identifier(struct kefir_mem *mem,
                                                       struct kefir_ir_module *module,
                                                       const char *identifier,
                                                       const struct kefir_ast_scoped_identifier *scoped_identifier,
                                                       struct kefir_ast_global_scope_layout *layout) {
    switch (scoped_identifier->object.storage) {
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN:
            REQUIRE_OK(kefir_hashtree_insert(mem, &layout->external_objects,
                (kefir_hashtree_key_t) identifier, (kefir_hashtree_value_t) NULL));
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO: {
            struct kefir_ir_type *type = kefir_ir_module_new_type(mem, module, 0, NULL);
            struct kefir_irbuilder_type builder;
            REQUIRE_OK(kefir_irbuilder_type_init(mem, &builder, type));
            REQUIRE_OK(kefir_ast_translate_type(scoped_identifier->object.type, &builder, NULL));
            REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder));
            REQUIRE_OK(kefir_hashtree_insert(mem, &layout->external_objects,
                (kefir_hashtree_key_t) identifier, (kefir_hashtree_value_t) type));
        } break;

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL: {
            struct kefir_ir_type *type = kefir_ir_module_new_type(mem, module, 0, NULL);
            struct kefir_irbuilder_type builder;
            REQUIRE_OK(kefir_irbuilder_type_init(mem, &builder, type));
            REQUIRE_OK(kefir_ast_translate_type(scoped_identifier->object.type, &builder, NULL));
            REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder));
            REQUIRE_OK(kefir_hashtree_insert(mem, &layout->external_thread_local_objects,
                (kefir_hashtree_key_t) identifier, (kefir_hashtree_value_t) type));
        } break;

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL: {
            REQUIRE_OK(kefir_hashtree_insert(mem, &layout->external_thread_local_objects,
                (kefir_hashtree_key_t) identifier, (kefir_hashtree_value_t) NULL));
        } break;

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC: {
            const kefir_size_t index = kefir_ir_type_total_length(layout->static_layout);
            struct kefir_irbuilder_type builder;
            REQUIRE_OK(kefir_irbuilder_type_init(mem, &builder, layout->static_layout));
            REQUIRE_OK(kefir_ast_translate_type(scoped_identifier->object.type, &builder, NULL));
            REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder));
            REQUIRE_OK(kefir_hashtree_insert(mem, &layout->static_objects,
                (kefir_hashtree_key_t) identifier, (kefir_hashtree_value_t) index));
        } break;
        
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL: {
            const kefir_size_t index = kefir_ir_type_total_length(layout->static_thread_local_layout);
            struct kefir_irbuilder_type builder;
            REQUIRE_OK(kefir_irbuilder_type_init(mem, &builder, layout->static_thread_local_layout));
            REQUIRE_OK(kefir_ast_translate_type(scoped_identifier->object.type, &builder, NULL));
            REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder));
            REQUIRE_OK(kefir_hashtree_insert(mem, &layout->static_thread_local_objects,
                (kefir_hashtree_key_t) identifier, (kefir_hashtree_value_t) index));
        } break;

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "File-scope variable cannot have register storage");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_global_scope_layout(struct kefir_mem *mem,
                                                   struct kefir_ir_module *module,
                                                   const struct kefir_ast_global_context *context,
                                                   struct kefir_ast_global_scope_layout *layout) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST global context"));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST global scope layout"));
    struct kefir_ast_identifier_flat_scope_iterator iter;
    kefir_result_t res;
    for (res = kefir_ast_identifier_flat_scope_iter(&context->object_scope, &iter);
        res == KEFIR_OK;
        res = kefir_ast_identifier_flat_scope_next(&context->object_scope, &iter)) {
        REQUIRE_OK(translate_global_scoped_identifier(mem, module, iter.identifier, iter.value, layout));
    }
    REQUIRE_ELSE(res == KEFIR_ITERATOR_END, {
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t translate_local_scoped_identifier(struct kefir_mem *mem,
                                                      struct kefir_irbuilder_type *builder,
                                                      const char *identifier,
                                                      const struct kefir_ast_scoped_identifier *scoped_identifier,
                                                      struct kefir_ast_local_scope_layout *local_layout,
                                                      struct kefir_ast_global_scope_layout *global_layout) {
    switch (scoped_identifier->object.storage) {
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN:
            REQUIRE_OK(kefir_hashtree_insert(mem, &global_layout->external_objects,
                (kefir_hashtree_key_t) identifier, (kefir_hashtree_value_t) NULL));
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC: {
            const kefir_size_t index = kefir_ir_type_total_length(global_layout->static_layout);
            struct kefir_irbuilder_type global_builder;
            REQUIRE_OK(kefir_irbuilder_type_init(mem, &global_builder, global_layout->static_layout));
            REQUIRE_OK(kefir_ast_translate_type(scoped_identifier->object.type, &global_builder, NULL));
            REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&global_builder));
            REQUIRE_OK(kefir_hashtree_insert(mem, &local_layout->static_locals,
                (kefir_hashtree_key_t) identifier, (kefir_hashtree_value_t) index));
        } break;

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot have thread block-scope variable with no linkage");

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL:
            REQUIRE_OK(kefir_hashtree_insert(mem, &global_layout->external_thread_local_objects,
                (kefir_hashtree_key_t) identifier, (kefir_hashtree_value_t) NULL));
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL: {
            const kefir_size_t index = kefir_ir_type_total_length(global_layout->static_thread_local_layout);
            struct kefir_irbuilder_type global_builder;
            REQUIRE_OK(kefir_irbuilder_type_init(mem, &global_builder, global_layout->static_thread_local_layout));
            REQUIRE_OK(kefir_ast_translate_type(scoped_identifier->object.type, &global_builder, NULL));
            REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&global_builder));
            REQUIRE_OK(kefir_hashtree_insert(mem, &local_layout->static_thread_local_locals,
                (kefir_hashtree_key_t) identifier, (kefir_hashtree_value_t) index));
        } break;

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER:
            REQUIRE_OK(kefir_ast_translate_type(scoped_identifier->object.type, builder, NULL));
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t traverse_local_scope(struct kefir_mem *mem,
                                         const struct kefir_tree_node *root,
                                         struct kefir_irbuilder_type *builder,
                                         struct kefir_ast_local_scope_layout *local_layout,
                                         struct kefir_ast_global_scope_layout *global_layout) {
    ASSIGN_DECL_CAST(struct kefir_ast_identifier_flat_scope *, scope,
        root->value);
    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_STRUCT, 0, 0));
    struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(builder->type, kefir_ir_type_total_length(builder->type) - 1);
    kefir_size_t scope_length = 0;
    struct kefir_ast_identifier_flat_scope_iterator iter;
    kefir_result_t res;
    for (res = kefir_ast_identifier_flat_scope_iter(scope, &iter);
        res == KEFIR_OK;
        res = kefir_ast_identifier_flat_scope_next(scope, &iter)) {
        scope_length++;
        REQUIRE_OK(translate_local_scoped_identifier(mem, builder, iter.identifier, iter.value, local_layout, global_layout));
    }
    REQUIRE_ELSE(res == KEFIR_ITERATOR_END, {
        return res;
    });
    if (kefir_tree_first_child(root) != NULL) {
        scope_length++;
        REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_UNION, 0, 0));
        struct kefir_ir_typeentry *child_typeentry =
            kefir_ir_type_at(builder->type, kefir_ir_type_total_length(builder->type) - 1);
        kefir_size_t children = 0;
        for (struct kefir_tree_node *child = kefir_tree_first_child(root);
            child != NULL;
            child = kefir_tree_next_sibling(child)) {
            children++;
            REQUIRE_OK(traverse_local_scope(mem, child, builder, local_layout, global_layout));
        }
        child_typeentry->param = children;
    }
    typeentry->param = scope_length;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_local_scope_layout(struct kefir_mem *mem,
                                                  const struct kefir_ast_context *context,
                                                  struct kefir_ast_local_scope_layout *layout) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST local scope layout"));

    struct kefir_irbuilder_type builder;
    REQUIRE_OK(kefir_irbuilder_type_init(mem, &builder, layout->locals));
    REQUIRE_OK(traverse_local_scope(mem, &context->local_object_scope.root, &builder, layout, layout->global));
    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder));
    return KEFIR_OK;
}