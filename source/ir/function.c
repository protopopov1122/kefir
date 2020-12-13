#include <stdlib.h>
#include <string.h>
#include "kefir/ir/function.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_irfunction_init(struct kefir_irfunction *func, const char *identifier) {
    REQUIRE(func != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function identifier"));
    REQUIRE(strlen(identifier) > 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function identifier"));
    kefir_result_t result = kefir_ir_type_init(&func->declaration.params, NULL, 0);
    REQUIRE(result == KEFIR_OK, result);
    result = kefir_ir_type_init(&func->declaration.result, NULL, 0);
    REQUIRE(result == KEFIR_OK, result);
    result = kefir_irblock_init(&func->body, NULL, 0);
    REQUIRE(result == KEFIR_OK, result);
    func->declaration.identifier = identifier;
    return KEFIR_OK;
}

kefir_result_t kefir_irfunction_alloc(struct kefir_mem *mem,
                                  const char *ident,
                                  kefir_size_t sigSz,
                                  kefir_size_t retSigSz,
                                  kefir_size_t bodySz,
                                  struct kefir_irfunction *func) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocation"));
    REQUIRE(ident != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function identifier"));
    REQUIRE(strlen(ident) > 0, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function identifier"));
    REQUIRE(func != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function pointer"));
    char *identifier = KEFIR_MALLOC(mem, strlen(ident) + 1);
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate memory for IR function identifier"));
    strcpy(identifier, ident);
    func->declaration.identifier = identifier;
    kefir_result_t result = kefir_ir_type_alloc(mem, sigSz, &func->declaration.params);
    REQUIRE_ELSE(result == KEFIR_OK, {
        KEFIR_FREE(mem, (char *) func->declaration.identifier);
        return result;
    });
    result = kefir_ir_type_alloc(mem, retSigSz, &func->declaration.result);
    REQUIRE_ELSE(result == KEFIR_OK, {
        KEFIR_FREE(mem, (char *) func->declaration.identifier);
        kefir_ir_type_free(mem, &func->declaration.params);
        return result;
    });
    result = kefir_irblock_alloc(mem, bodySz, &func->body);
    REQUIRE_ELSE(result == KEFIR_OK, {
        KEFIR_FREE(mem, (char *) func->declaration.identifier);
        kefir_ir_type_free(mem, &func->declaration.params);
        kefir_ir_type_free(mem, &func->declaration.result);
        return result;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_irfunction_free(struct kefir_mem *mem, struct kefir_irfunction *func) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(func != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function pointer"));
    KEFIR_FREE(mem, (char *) func->declaration.identifier);
    func->declaration.identifier = NULL;
    kefir_ir_type_free(mem, &func->declaration.params);
    kefir_ir_type_free(mem, &func->declaration.result);
    kefir_irblock_free(mem, &func->body);
    return KEFIR_OK;
}
