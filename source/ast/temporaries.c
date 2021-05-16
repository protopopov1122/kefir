#include <stdio.h>
#include "kefir/ast/temporaries.h"
#include "kefir/ast/runtime.h"
#include "kefir/ast/context.h"
#include "kefir/ast/type.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_bool_t kefir_ast_temporaries_init(struct kefir_mem *mem,
                                    struct kefir_ast_type_bundle *type_bundle,
                                    struct kefir_ast_context_temporaries *temporaries) {
    REQUIRE(temporaries != NULL, false);
    if (temporaries->type == NULL) {
        temporaries->type = kefir_ast_type_union(mem, type_bundle,
            "", &temporaries->temporaries);
        REQUIRE(temporaries->type != NULL &&
            temporaries->temporaries != NULL,
            false);
        temporaries->current.identifier = 0;
        temporaries->current.field = 0;
        return true;
    }
    return false;
}

kefir_result_t kefir_ast_temporaries_next_block(struct kefir_ast_context_temporaries *temporaries) {
    REQUIRE(temporaries != NULL, KEFIR_OK);
    if (temporaries->temporaries != NULL) {
        temporaries->current.identifier++;
        temporaries->current.field = 0;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_temporaries_new_temporary(struct kefir_mem *mem,
                                               const struct kefir_ast_context *context,
                                               const struct kefir_ast_type *type,
                                               struct kefir_ast_temporary_identifier *temp_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid temporary AST type"));
    REQUIRE(temp_id != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid temporary identifier pointer"));

#define BUFFER_LEN 128
    char BUFFER[BUFFER_LEN] = {0};
    snprintf(BUFFER, BUFFER_LEN - 1, KEFIR_AST_TRANSLATOR_TEMPORARY_VALUE_IDENTIFIER,
        context->temporaries->current.identifier);

    struct kefir_ast_struct_type *temporary_value = NULL;
    const struct kefir_ast_struct_field *temporary_value_field = NULL;
    kefir_result_t res = kefir_ast_struct_type_get_field(context->temporaries->temporaries,
        BUFFER, &temporary_value_field);
    if (res == KEFIR_NOT_FOUND) {
        const struct kefir_ast_type *temporary_value_type = kefir_ast_type_structure(mem, context->type_bundle,
            BUFFER, &temporary_value);
        REQUIRE(temporary_value_type != NULL,
            KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate a temporary value type"));
        REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, context->temporaries->temporaries,
            BUFFER, temporary_value_type, NULL));
    } else {
        REQUIRE_OK(res);
        temporary_value = (struct kefir_ast_struct_type *) &temporary_value_field->type->structure_type;
    }

    snprintf(BUFFER, BUFFER_LEN - 1, KEFIR_AST_TRANSLATOR_TEMPORARY_MEMBER_IDENTIFIER,
        context->temporaries->current.field);
#undef BUFFER_LEN

    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, temporary_value,
        BUFFER, type, NULL));

    *temp_id = context->temporaries->current;
    context->temporaries->current.field++;
    return KEFIR_OK;
}
