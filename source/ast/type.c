#include <string.h>
#include "kefir/ast/type.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_size_t default_integral_type_fit_rank(const struct kefir_ast_type *type) {
    switch (type->tag) {
        case KEFIR_AST_TYPE_SCALAR_BOOL:
        case KEFIR_AST_TYPE_SCALAR_CHAR:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
            return 1;

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
            return 2;

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
            return 4;

        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
            return 8;
        
        default:
            return 0;
    }
}

static kefir_result_t default_integral_type_fits(const struct kefir_ast_type_traits *type_traits,
                                               const struct kefir_ast_type *source,
                                               const struct kefir_ast_type *dest,
                                               kefir_bool_t *result) {
    UNUSED(type_traits);
    REQUIRE(source != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid source AST type"));
    REQUIRE(dest != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid destination AST type"));
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid result pointer"));
    REQUIRE((KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(source) || source->tag == KEFIR_AST_TYPE_SCALAR_BOOL) &&
        (KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(dest) || dest->tag == KEFIR_AST_TYPE_SCALAR_BOOL),
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected both source and destination to be basic types"));
    kefir_size_t source_fit = default_integral_type_fit_rank(source);
    kefir_size_t dest_fit = default_integral_type_fit_rank(dest);
    REQUIRE(source_fit != 0 && dest_fit != 0,
        KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected integral type"));
    if (source->basic_type.signedness == dest->basic_type.signedness ||
        (source->basic_type.signedness && !dest->basic_type.signedness)) {
        *result = source_fit <= dest_fit;
    } else if (!source->basic_type.signedness && dest->basic_type.signedness) {
        *result = source_fit < dest_fit;
    } else {
        *result = false;
    }
    return KEFIR_OK;
}

const struct kefir_ast_type_traits DEFAULT_TYPE_TRAITS = {
    .integral_type_fits = default_integral_type_fits,
    .payload = NULL
};

const struct kefir_ast_type_traits *kefir_ast_default_type_traits() {
    return &DEFAULT_TYPE_TRAITS;
}

static kefir_result_t free_type_storage(struct kefir_mem *mem,
                                   struct kefir_list *list,
                                   struct kefir_list_entry *entry,
                                   void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid list entry"));
    REQUIRE_OK(KEFIR_AST_TYPE_FREE(mem, (struct kefir_ast_type *) entry->value));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_storage_init(struct kefir_ast_type_storage *type_storage,
                                          struct kefir_symbol_table *symbols) {
    REQUIRE(type_storage != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type type_storagesitory"));
    type_storage->symbols = symbols;
    REQUIRE_OK(kefir_list_init(&type_storage->types));
    REQUIRE_OK(kefir_list_on_remove(&type_storage->types, free_type_storage, NULL));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_type_storage_free(struct kefir_mem *mem, struct kefir_ast_type_storage *type_storage) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type_storage != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type type_storagesitory"));
    REQUIRE_OK(kefir_list_free(mem, &type_storage->types));
    return KEFIR_OK;
}
