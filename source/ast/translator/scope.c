#include "kefir/ast/translator/scope.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_global_scope_layout_init(struct kefir_mem *mem,
                                              struct kefir_ir_module *module,
                                              struct kefir_ast_global_scope_layout *layout) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR module"));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST global scope layout"));
    REQUIRE_OK(kefir_hashtree_init(&layout->objects, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_hashtree_init(&layout->thread_local_objects, &kefir_hashtree_str_ops));
    layout->static_objects = kefir_ir_module_new_type(mem, module, 0, NULL);
    layout->thread_local_static_objects = kefir_ir_module_new_type(mem, module, 0, NULL);
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
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_scope_layout_free(struct kefir_mem *mem,
                                              struct kefir_ast_global_scope_layout *layout) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST global scope layout"));
    REQUIRE_OK(kefir_hashtree_free(mem, &layout->objects));
    REQUIRE_OK(kefir_hashtree_free(mem, &layout->thread_local_objects));
    layout->static_objects = NULL;
    layout->thread_local_static_objects = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_lobal_scope_layout_free(struct kefir_ast_local_scope_layout *layout) {
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST local scope layout"));
    layout->global = NULL;
    layout->locals = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_global_scope_layout(struct kefir_mem *mem,
                                                   const struct kefir_ast_global_context *context,
                                                   struct kefir_ast_global_scope_layout *layout) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST global context"));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST global scope layout"));
    struct kefir_ast_identifier_flat_scope_iterator iter;
    kefir_result_t res;
    for (res = kefir_ast_identifier_flat_scope_iter(&context->object_scope, &iter);
        res == KEFIR_OK;
        res = kefir_ast_identifier_flat_scope_next(&context->object_scope, &iter)) {
        switch (iter.value->object.storage) {
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL:
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC:
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL:
                break;
            
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL:
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER:
                break;
        }
    }
    REQUIRE_ELSE(res == KEFIR_ITERATOR_END, {
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_local_scope_layout(struct kefir_mem *mem,
                                                  const struct kefir_ast_context *context,
                                                  struct kefir_ast_local_scope_layout *layout) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST local scope layout"));
    return KEFIR_OK;
}
