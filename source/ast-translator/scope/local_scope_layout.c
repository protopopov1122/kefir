#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/ast-translator/scope/scope_layout_impl.h"

kefir_result_t kefir_ast_translator_local_scope_layout_init(struct kefir_mem *mem,
                                             struct kefir_ir_module *module,
                                             struct kefir_ast_translator_global_scope_layout *global,
                                             struct kefir_ast_translator_local_scope_layout *layout) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module"));
    REQUIRE(global != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST global scope layout"));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST local scope layout"));
    REQUIRE_OK(kefir_list_init(&layout->local_objects));
    REQUIRE_OK(kefir_list_init(&layout->static_objects));
    REQUIRE_OK(kefir_list_init(&layout->static_thread_local_objects));
    REQUIRE_OK(kefir_list_on_remove(&layout->local_objects, kefir_ast_translator_scoped_identifier_remove, NULL));
    REQUIRE_OK(kefir_list_on_remove(&layout->static_objects, kefir_ast_translator_scoped_identifier_remove, NULL));
    REQUIRE_OK(kefir_list_on_remove(&layout->static_thread_local_objects, kefir_ast_translator_scoped_identifier_remove, NULL));
    layout->global = global;
    layout->local_layout = kefir_ir_module_new_type(mem, module, 0, &layout->local_layout_id);
    REQUIRE(layout->local_layout != NULL,
        KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate new IR type"));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_local_scope_layout_free(struct kefir_mem *mem,
                                             struct kefir_ast_translator_local_scope_layout *layout) {
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST local scope layout"));
    REQUIRE_OK(kefir_list_free(mem, &layout->local_objects));
    REQUIRE_OK(kefir_list_free(mem, &layout->static_objects));
    REQUIRE_OK(kefir_list_free(mem, &layout->static_thread_local_objects));
    layout->global = NULL;
    layout->local_layout = NULL;
    return KEFIR_OK;
}

static kefir_result_t translate_static_identifier(struct kefir_mem *mem,
                                                const struct kefir_ast_translator_environment *env,
                                                struct kefir_ast_translator_local_scope_layout *local_layout,
                                                const char *identifier,
                                                const struct kefir_ast_scoped_identifier *scoped_identifier) {
    ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_layout *, scoped_identifier_layout,
        scoped_identifier->payload.ptr);
    KEFIR_AST_SCOPE_SET_CLEANUP(scoped_identifier, kefir_ast_translator_scoped_identifer_payload_free, NULL);
    struct kefir_irbuilder_type global_builder;
    REQUIRE_OK(kefir_irbuilder_type_init(mem, &global_builder, local_layout->global->static_layout));
    kefir_result_t res = kefir_ast_translate_object_type(mem, scoped_identifier->object.type,
        scoped_identifier->object.alignment->value, env, &global_builder, &scoped_identifier_layout->layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_IRBUILDER_TYPE_FREE(&global_builder);
        return res;
    });
    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&global_builder));
    scoped_identifier_layout->type_id = local_layout->global->static_layout_id;
    scoped_identifier_layout->type = local_layout->global->static_layout;
    REQUIRE_OK(kefir_ast_translator_scoped_identifier_insert(mem, identifier, scoped_identifier, &local_layout->static_objects));
    return KEFIR_OK;
}

static kefir_result_t translate_static_thread_local_identifier(struct kefir_mem *mem,
                                                             const struct kefir_ast_translator_environment *env,
                                                             struct kefir_ast_translator_local_scope_layout *local_layout,
                                                             const char *identifier,
                                                             const struct kefir_ast_scoped_identifier *scoped_identifier) {
    ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_layout *, scoped_identifier_layout,
        scoped_identifier->payload.ptr);
    KEFIR_AST_SCOPE_SET_CLEANUP(scoped_identifier, kefir_ast_translator_scoped_identifer_payload_free, NULL);
    struct kefir_irbuilder_type global_builder;
    REQUIRE_OK(kefir_irbuilder_type_init(mem, &global_builder, local_layout->global->static_thread_local_layout));
    kefir_result_t res = kefir_ast_translate_object_type(mem, scoped_identifier->object.type,
        scoped_identifier->object.alignment->value, env, &global_builder, &scoped_identifier_layout->layout);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_IRBUILDER_TYPE_FREE(&global_builder);
        return res;
    });
    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&global_builder));
    scoped_identifier_layout->type_id = local_layout->global->static_thread_local_layout_id;
    scoped_identifier_layout->type = local_layout->global->static_thread_local_layout;
    REQUIRE_OK(kefir_ast_translator_scoped_identifier_insert(mem, identifier, scoped_identifier,
        &local_layout->static_thread_local_objects));
    return KEFIR_OK;
}

static kefir_result_t translate_auto_register_identifier(struct kefir_mem *mem,
                                                       struct kefir_irbuilder_type *builder,
                                                       const struct kefir_ast_translator_environment *env,
                                                       struct kefir_ast_translator_local_scope_layout *local_layout,
                                                       const char *identifier,
                                                       const struct kefir_ast_scoped_identifier *scoped_identifier) {
    ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_layout *, scoped_identifier_layout,
        scoped_identifier->payload.ptr);
    KEFIR_AST_SCOPE_SET_CLEANUP(scoped_identifier, kefir_ast_translator_scoped_identifer_payload_free, NULL);
    REQUIRE_OK(kefir_ast_translate_object_type(mem, scoped_identifier->object.type,
        scoped_identifier->object.alignment->value, env, builder, &scoped_identifier_layout->layout));
    scoped_identifier_layout->type_id = local_layout->local_layout_id;
    scoped_identifier_layout->type = builder->type;
    REQUIRE_OK(kefir_ast_translator_scoped_identifier_insert(mem, identifier, scoped_identifier,
        &local_layout->local_objects));
    return KEFIR_OK;
}

static kefir_result_t translate_local_scoped_identifier(struct kefir_mem *mem,
                                                      struct kefir_irbuilder_type *builder,
                                                      const char *identifier,
                                                      const struct kefir_ast_scoped_identifier *scoped_identifier,
                                                      const struct kefir_ast_translator_environment *env,
                                                      struct kefir_ast_translator_local_scope_layout *local_layout) {
    REQUIRE(scoped_identifier->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT, KEFIR_OK);
    switch (scoped_identifier->object.storage) {
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL:
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC:
            REQUIRE_OK(translate_static_identifier(mem, env, local_layout, identifier, scoped_identifier));
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot have thread local block-scope variable with no linkage");

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL:
            REQUIRE_OK(translate_static_thread_local_identifier(mem, env, local_layout, identifier, scoped_identifier));
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER:
            REQUIRE_OK(translate_auto_register_identifier(mem, builder, env, local_layout, identifier, scoped_identifier));
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t traverse_local_scope(struct kefir_mem *mem,
                                         const struct kefir_tree_node *root,
                                         struct kefir_irbuilder_type *builder,
                                         const struct kefir_ast_translator_environment *env,
                                         struct kefir_ast_translator_local_scope_layout *local_layout) {
    ASSIGN_DECL_CAST(struct kefir_ast_identifier_flat_scope *, scope,
        root->value);
    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_STRUCT, 0, 0));
    const kefir_size_t begin = kefir_ir_type_total_length(builder->type) - 1;
    kefir_size_t scope_length = 0;
    struct kefir_ast_identifier_flat_scope_iterator iter;
    kefir_result_t res;
    for (res = kefir_ast_identifier_flat_scope_iter(scope, &iter);
        res == KEFIR_OK;
        res = kefir_ast_identifier_flat_scope_next(scope, &iter)) {
        scope_length++;
        REQUIRE_OK(translate_local_scoped_identifier(mem, builder, iter.identifier, iter.value, env, local_layout));
    }
    REQUIRE(res == KEFIR_ITERATOR_END, res);
    if (kefir_tree_first_child(root) != NULL) {
        scope_length++;
        REQUIRE_OK(KEFIR_IRBUILDER_TYPE_APPEND_V(builder, KEFIR_IR_TYPE_UNION, 0, 0));
        const kefir_size_t child_begin = kefir_ir_type_total_length(builder->type) - 1;
        kefir_size_t children = 0;
        for (struct kefir_tree_node *child = kefir_tree_first_child(root);
            child != NULL;
            child = kefir_tree_next_sibling(child)) {
            children++;
            REQUIRE_OK(traverse_local_scope(mem, child, builder, env, local_layout));
        }
        struct kefir_ir_typeentry *child_typeentry =
            kefir_ir_type_at(builder->type, child_begin);
        child_typeentry->param = children;
    }
    struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(builder->type, begin);
    typeentry->param = scope_length;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_build_local_scope_layout(struct kefir_mem *mem,
                                           const struct kefir_ast_local_context *context,
                                           const struct kefir_ast_translator_environment *env,
                                           struct kefir_ast_translator_local_scope_layout *layout) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST local scope layout"));

    struct kefir_irbuilder_type builder;
    REQUIRE_OK(kefir_irbuilder_type_init(mem, &builder, layout->local_layout));
    REQUIRE_OK(traverse_local_scope(mem, &context->ordinary_scope.root, &builder, env, layout));
    REQUIRE_OK(KEFIR_IRBUILDER_TYPE_FREE(&builder));
    return KEFIR_OK;
}