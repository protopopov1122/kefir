#include "kefir/ast/type.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_bool_t same_pointer_type(const struct kefir_ast_type *type1, const struct kefir_ast_type *type2) {
    REQUIRE(type1 != NULL, false);
    REQUIRE(type2 != NULL, false);
    REQUIRE(type1->tag == KEFIR_AST_TYPE_SCALAR_POINTER &&
        type2->tag == KEFIR_AST_TYPE_SCALAR_POINTER, false);
    return KEFIR_AST_TYPE_SAME(type1->referenced_type, type2->referenced_type);
}

static kefir_result_t free_pointer_type(struct kefir_mem *mem, const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    KEFIR_FREE(mem, (void *) type);
    return KEFIR_OK;
}

const struct kefir_ast_type *kefir_ast_type_pointer(struct kefir_mem *mem,
                                                struct kefir_ast_type_storage *type_storage,
                                                const struct kefir_ast_type *base_type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base_type != NULL, NULL);
    struct kefir_ast_type *type = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_type));
    REQUIRE(type != NULL, NULL);
    if (type_storage != NULL) {
        kefir_result_t res = kefir_list_insert_after(mem, &type_storage->types, kefir_list_tail(&type_storage->types), type);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, type);
            return NULL;
        });
    }
    type->tag = KEFIR_AST_TYPE_SCALAR_POINTER;
    type->basic = false;
    type->ops.same = same_pointer_type;
    type->ops.free = free_pointer_type;
    type->referenced_type = base_type;
    return type;
}

const struct kefir_ast_type *kefir_ast_pointer_referenced_type(const struct kefir_ast_type *pointer_type) {
    REQUIRE(pointer_type != NULL, NULL);
    return pointer_type->referenced_type;
}