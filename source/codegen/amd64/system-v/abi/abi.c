#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/amd64/system-v/runtime.h"
#include "kefir/codegen/amd64/system-v/abi/data_layout.h"
#include "kefir/codegen/amd64/system-v/abi/registers.h"
#include "kefir/codegen/util.h"
#include <stdio.h>

static kefir_result_t frame_parameter_visitor(const struct kefir_ir_type *type,
                                            kefir_size_t index,
                                            const struct kefir_ir_typeentry *typeentry,
                                            void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    struct kefir_amd64_sysv_function *sysv_func =
        (struct kefir_amd64_sysv_function *) payload;
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, alloc,
        kefir_vector_at(&sysv_func->decl.parameters.allocation, iter.slot));
    if (alloc->type == KEFIR_AMD64_SYSV_INPUT_PARAM_OWNING_CONTAINER) {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout,
            kefir_vector_at(&sysv_func->decl.parameters.layout, index));
        sysv_func->frame.alignment = MAX(sysv_func->frame.alignment, layout->alignment);
        sysv_func->frame.size = kefir_codegen_pad_aligned(sysv_func->frame.size, sysv_func->frame.alignment);
        sysv_func->frame.size += layout->size;
    }
    return KEFIR_OK;
}

static kefir_result_t calculate_frame(struct kefir_amd64_sysv_function *sysv_func) {
    sysv_func->frame.size = 0;
    sysv_func->frame.alignment = 0;
    sysv_func->frame.base.internals = 0;
    for (kefir_size_t i = 0; i < KEFIR_AMD64_SYSV_INTERNAL_COUNT; i++) {
        if (sysv_func->internals[i].enabled) {
            sysv_func->internals[i].offset = sysv_func->frame.size;
            sysv_func->frame.size += KEFIR_AMD64_SYSV_ABI_QWORD;
            sysv_func->frame.alignment = KEFIR_AMD64_SYSV_ABI_QWORD;
        }
    }
    sysv_func->frame.base.parameters = sysv_func->frame.size;
    struct kefir_ir_type_visitor visitor;
    kefir_ir_type_visitor_init(&visitor, frame_parameter_visitor);
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(
        sysv_func->func->declaration->params, &visitor, (void *) sysv_func, 0,
        kefir_ir_type_nodes(sysv_func->func->declaration->params)));
    sysv_func->frame.size = kefir_codegen_pad_aligned(sysv_func->frame.size, KEFIR_AMD64_SYSV_ABI_QWORD);
    sysv_func->frame.base.locals = sysv_func->frame.size;
    return KEFIR_OK;
}

static kefir_result_t function_alloc_return(struct kefir_mem *mem,
                                          struct kefir_amd64_sysv_function_decl *sysv_decl) {
    REQUIRE(kefir_ir_type_nodes(sysv_decl->decl->result) <= 1,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected IR function to have return type count less than 2"));
    REQUIRE_OK(kefir_amd64_sysv_type_layout(sysv_decl->decl->result,
        mem, &sysv_decl->returns.layout));
    kefir_result_t res = kefir_amd64_sysv_parameter_classify(mem,
        sysv_decl->decl->result, &sysv_decl->returns.layout, &sysv_decl->returns.allocation);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_vector_free(mem, &sysv_decl->returns.layout);
        return res;
    });
    sysv_decl->returns.implicit_parameter = false;
    if (kefir_ir_type_nodes(sysv_decl->decl->result) > 0) {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, result,
            kefir_vector_at(&sysv_decl->returns.allocation, 0));
        sysv_decl->returns.implicit_parameter = result->klass == KEFIR_AMD64_SYSV_PARAM_MEMORY;
    }
    return KEFIR_OK;
}

static kefir_result_t function_alloc_params(struct kefir_mem *mem,
                                          struct kefir_amd64_sysv_function_decl *sysv_decl) {
    REQUIRE_OK(kefir_amd64_sysv_type_layout(sysv_decl->decl->params,
        mem, &sysv_decl->parameters.layout));
    
    kefir_result_t res = kefir_amd64_sysv_parameter_classify(mem,
        sysv_decl->decl->params, &sysv_decl->parameters.layout, &sysv_decl->parameters.allocation);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_vector_free(mem, &sysv_decl->parameters.layout);
        return res;
    });
    sysv_decl->parameters.location = (struct kefir_amd64_sysv_parameter_location){0};
    if (sysv_decl->returns.implicit_parameter) {
        sysv_decl->parameters.location.integer_register++;
    }
    res = kefir_amd64_sysv_parameter_allocate(mem,
        sysv_decl->decl->params, &sysv_decl->parameters.layout,
        &sysv_decl->parameters.allocation, &sysv_decl->parameters.location);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_amd64_sysv_parameter_free(mem, &sysv_decl->parameters.allocation);
        kefir_vector_free(mem, &sysv_decl->parameters.layout);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_function_decl_alloc(struct kefir_mem *mem,
                                                const struct kefir_ir_function_decl *decl,
                                                struct kefir_amd64_sysv_function_decl *sysv_decl) {
    sysv_decl->decl = decl;
    REQUIRE_OK(function_alloc_return(mem, sysv_decl));
    kefir_result_t res = function_alloc_params(mem, sysv_decl);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_amd64_sysv_parameter_free(mem, &sysv_decl->returns.allocation);
        kefir_vector_free(mem, &sysv_decl->returns.layout);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_function_decl_free(struct kefir_mem *mem,
                                               struct kefir_amd64_sysv_function_decl *sysv_decl) {
    REQUIRE_OK(kefir_amd64_sysv_parameter_free(mem, &sysv_decl->returns.allocation));
    REQUIRE_OK(kefir_vector_free(mem, &sysv_decl->returns.layout));
    REQUIRE_OK(kefir_amd64_sysv_parameter_free(mem, &sysv_decl->parameters.allocation));
    REQUIRE_OK(kefir_vector_free(mem, &sysv_decl->parameters.layout));
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_function_alloc(struct kefir_mem *mem,
                                           const struct kefir_ir_function *func,
                                           struct kefir_amd64_sysv_function *sysv_func) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(func != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR function"));
    REQUIRE(sysv_func != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 System-V function"));
    for (kefir_size_t i = 0; i < KEFIR_AMD64_SYSV_INTERNAL_COUNT; i++) {
        sysv_func->internals[i].enabled = false;
        sysv_func->internals[i].offset = 0;
    }
    sysv_func->func = func;
    REQUIRE_OK(kefir_amd64_sysv_function_decl_alloc(mem, func->declaration, &sysv_func->decl));
    if (sysv_func->decl.returns.implicit_parameter) {
        sysv_func->internals[KEFIR_AMD64_SYSV_INTERNAL_RETURN_ADDRESS].enabled = true;
    }
    kefir_result_t res = calculate_frame(sysv_func);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_amd64_sysv_parameter_free(mem, &sysv_func->decl.returns.allocation);
        kefir_vector_free(mem, &sysv_func->decl.returns.layout);
        kefir_amd64_sysv_parameter_free(mem, &sysv_func->decl.parameters.allocation);
        kefir_vector_free(mem, &sysv_func->decl.parameters.layout);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_function_free(struct kefir_mem *mem, struct kefir_amd64_sysv_function *sysv_func) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(sysv_func != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 System-V function"));
    REQUIRE_OK(kefir_amd64_sysv_function_decl_free(mem, &sysv_func->decl));
    return KEFIR_OK;
}