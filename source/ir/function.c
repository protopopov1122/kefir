#include <stdlib.h>
#include <string.h>
#include "kefir/ir/function.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ir_function_decl_alloc(struct kefir_mem *mem,
                                       const char *identifier,
                                       struct kefir_ir_type *parameters,
                                       struct kefir_ir_type *returns,
                                       struct kefir_ir_function_decl *decl) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocation"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function identifier"));
    REQUIRE(strlen(identifier) > 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function identifier"));
    REQUIRE(decl != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function declaration pointer"));
    char *identifier_copy = KEFIR_MALLOC(mem, strlen(identifier) + 1);
    REQUIRE(identifier_copy != NULL,
        KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate memory for IR function identifier"));
    strcpy(identifier_copy, identifier);
    decl->identifier = identifier_copy;
    decl->params = parameters;
    decl->result = returns;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_function_decl_free(struct kefir_mem *mem,
                                      struct kefir_ir_function_decl *decl) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocation"));;
    REQUIRE(decl != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function declaration pointer"));
    decl->result = NULL;
    decl->params = NULL;
    KEFIR_FREE(mem, (char *) decl->identifier);
    decl->identifier = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_function_alloc(struct kefir_mem *mem,
                                  struct kefir_ir_function_decl *decl,
                                  kefir_size_t bodySz,
                                  struct kefir_ir_function *func) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocation"));
    REQUIRE(func != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function pointer"));
    func->declaration = decl;
    kefir_result_t result = kefir_irblock_alloc(mem, bodySz, &func->body);
    REQUIRE_ELSE(result == KEFIR_OK, {
        kefir_ir_function_decl_free(mem, func->declaration);
        return result;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_ir_function_free(struct kefir_mem *mem, struct kefir_ir_function *func) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(func != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function pointer"));
    kefir_irblock_free(mem, &func->body);
    func->declaration = NULL;
    return KEFIR_OK;
}
