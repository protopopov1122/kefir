#include <stdlib.h>
#include <string.h>
#include "kefir/ir/function.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ir_function_decl_alloc(struct kefir_mem *mem,
                                       kefir_id_t id,
                                       const char *name,
                                       struct kefir_ir_type *parameters,
                                       bool vararg,
                                       struct kefir_ir_type *returns,
                                       struct kefir_ir_function_decl *decl) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocation"));
    REQUIRE(decl != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function declaration pointer"));
    decl->id = id;
    decl->name = name;
    decl->params = parameters;
    decl->result = returns;
    decl->vararg = vararg;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_function_decl_free(struct kefir_mem *mem,
                                      struct kefir_ir_function_decl *decl) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocation"));;
    REQUIRE(decl != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function declaration pointer"));
    decl->result = NULL;
    decl->params = NULL;
    decl->name = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_ir_function_alloc(struct kefir_mem *mem,
                                  struct kefir_ir_function_decl *decl,
                                  struct kefir_ir_type *locals,
                                  kefir_size_t bodySz,
                                  struct kefir_ir_function *func) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocation"));
    REQUIRE(func != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function pointer"));
    func->declaration = decl;
    func->locals = locals;
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
    func->locals = NULL;
    func->declaration = NULL;
    return KEFIR_OK;
}
