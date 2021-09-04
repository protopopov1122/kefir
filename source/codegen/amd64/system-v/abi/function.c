/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include <stdio.h>
#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/amd64/system-v/runtime.h"
#include "kefir/codegen/amd64/system-v/abi/data_layout.h"
#include "kefir/codegen/amd64/system-v/abi/registers.h"
#include "kefir/codegen/util.h"

static kefir_result_t frame_parameter_visitor(const struct kefir_ir_type *type, kefir_size_t index,
                                              const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    struct kefir_amd64_sysv_function *sysv_func = (struct kefir_amd64_sysv_function *) payload;
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

static kefir_result_t frame_local_visitor(const struct kefir_ir_type *type, kefir_size_t index,
                                          const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_function *, sysv_func, payload);
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout, kefir_vector_at(&sysv_func->local_layout, index));
    sysv_func->frame.alignment = MAX(sysv_func->frame.alignment, layout->alignment);
    sysv_func->frame.size = kefir_codegen_pad_aligned(sysv_func->frame.size, sysv_func->frame.alignment);
    sysv_func->frame.size += layout->size;
    return KEFIR_OK;
}

static kefir_result_t update_frame_temporaries(struct kefir_amd64_sysv_function_decl *decl, kefir_size_t *size,
                                               kefir_size_t *alignment) {
    if (kefir_ir_type_nodes(decl->decl->result) == 0) {
        return KEFIR_OK;
    }

    struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(decl->decl->result, 0);
    if (typeentry->typecode == KEFIR_IR_TYPE_STRUCT || typeentry->typecode == KEFIR_IR_TYPE_UNION ||
        typeentry->typecode == KEFIR_IR_TYPE_ARRAY || typeentry->typecode == KEFIR_IR_TYPE_MEMORY ||
        typeentry->typecode == KEFIR_IR_TYPE_BUILTIN) {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout, kefir_vector_at(&decl->returns.layout, 0));
        *size = MAX(*size, layout->size);
        *alignment = MAX(*alignment, layout->alignment);
    }
    return KEFIR_OK;
}

static kefir_result_t update_frame_temporaries_type(struct kefir_mem *mem, struct kefir_ir_type *type,
                                                    kefir_size_t index, kefir_size_t *size, kefir_size_t *alignment) {
    struct kefir_vector layout;
    struct kefir_vector allocation;
    struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(type, index);
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Unable to fetch IR type entry at index"));

    REQUIRE_OK(kefir_amd64_sysv_type_layout(type, mem, &layout));
    kefir_result_t res = kefir_amd64_sysv_parameter_classify(mem, type, &layout, &allocation);
    REQUIRE_ELSE(res == KEFIR_OK, {
        REQUIRE_OK(kefir_vector_free(mem, &layout));
        return res;
    });

    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, arg_layout, kefir_vector_at(&layout, index));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_parameter_allocation *, arg_alloc, kefir_vector_at(&allocation, index));
    REQUIRE_ELSE(arg_layout != NULL && arg_alloc != NULL, {
        REQUIRE_OK(kefir_amd64_sysv_parameter_free(mem, &allocation));
        REQUIRE_OK(kefir_vector_free(mem, &layout));
        return KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Unable to fetch argument layout and classification");
    });
    if ((typeentry->typecode == KEFIR_IR_TYPE_STRUCT || typeentry->typecode == KEFIR_IR_TYPE_UNION ||
         typeentry->typecode == KEFIR_IR_TYPE_ARRAY || typeentry->typecode == KEFIR_IR_TYPE_BUILTIN) &&
        arg_alloc->klass != KEFIR_AMD64_SYSV_PARAM_MEMORY) {
        *size = MAX(*size, arg_layout->size);
        *alignment = MAX(*size, arg_layout->alignment);
    }

    REQUIRE_OK(kefir_amd64_sysv_parameter_free(mem, &allocation));
    REQUIRE_OK(kefir_vector_free(mem, &layout));
    return KEFIR_OK;
}

#define PAD_DQWORD(x) kefir_codegen_pad_aligned((x), 2 * KEFIR_AMD64_SYSV_ABI_QWORD)

static kefir_result_t calculate_frame_temporaries(struct kefir_mem *mem,
                                                  struct kefir_codegen_amd64_sysv_module *sysv_module,
                                                  struct kefir_amd64_sysv_function *sysv_func) {
    kefir_size_t size = 0, alignment = 0;
    for (kefir_size_t i = 0; i < kefir_irblock_length(&sysv_func->func->body); i++) {
        const struct kefir_irinstr *instr = kefir_irblock_at(&sysv_func->func->body, i);
        REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected IR instructin at offset"));
        if (instr->opcode == KEFIR_IROPCODE_INVOKE) {
            kefir_id_t id = (kefir_id_t) instr->arg.u64;
            struct kefir_amd64_sysv_function_decl *decl =
                kefir_codegen_amd64_sysv_module_function_decl(mem, sysv_module, id, false);
            REQUIRE_OK(update_frame_temporaries(decl, &size, &alignment));
        }
        if (instr->opcode == KEFIR_IROPCODE_INVOKEV) {
            kefir_id_t id = (kefir_id_t) instr->arg.u64;
            struct kefir_amd64_sysv_function_decl *decl =
                kefir_codegen_amd64_sysv_module_function_decl(mem, sysv_module, id, true);
            REQUIRE_OK(update_frame_temporaries(decl, &size, &alignment));
        }
        if (instr->opcode == KEFIR_IROPCODE_VARARG_GET) {
            kefir_id_t id = (kefir_id_t) instr->arg.u32[0];
            struct kefir_ir_type *type = kefir_ir_module_get_named_type(sysv_module->module, id);
            REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unable to find named type"));
            REQUIRE_OK(update_frame_temporaries_type(mem, type, (kefir_size_t) instr->arg.u32[1], &size, &alignment));
        }
    }
    sysv_func->frame.base.temporary = kefir_codegen_pad_aligned(sysv_func->frame.size, alignment);
    sysv_func->frame.size = sysv_func->frame.base.temporary + size;
    sysv_func->frame.alignment = MAX(sysv_func->frame.alignment, alignment);
    return KEFIR_OK;
}

static kefir_result_t calculate_frame(struct kefir_mem *mem, struct kefir_codegen_amd64_sysv_module *sysv_module,
                                      struct kefir_amd64_sysv_function *sysv_func) {
    sysv_func->frame.size = KEFIR_AMD64_SYSV_INTERNAL_BOUND;
    sysv_func->frame.alignment = KEFIR_AMD64_SYSV_ABI_QWORD;
    sysv_func->frame.base.internals = 0;
    REQUIRE_OK(calculate_frame_temporaries(mem, sysv_module, sysv_func));
    sysv_func->frame.size = PAD_DQWORD(sysv_func->frame.size);
    sysv_func->frame.base.parameters = sysv_func->frame.size;
    struct kefir_ir_type_visitor visitor;
    kefir_ir_type_visitor_init(&visitor, frame_parameter_visitor);
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(sysv_func->func->declaration->params, &visitor, (void *) sysv_func, 0,
                                                kefir_ir_type_nodes(sysv_func->func->declaration->params)));
    sysv_func->frame.size = PAD_DQWORD(sysv_func->frame.size);
    if (sysv_func->func->declaration->vararg) {
        sysv_func->frame.base.register_save_area = sysv_func->frame.size;
        sysv_func->frame.size += KEFIR_AMD64_SYSV_ABI_QWORD * KEFIR_AMD64_SYSV_INTEGER_REGISTER_COUNT +
                                 2 * KEFIR_AMD64_SYSV_ABI_QWORD * KEFIR_AMD64_SYSV_SSE_REGISTER_COUNT;
        sysv_func->frame.size = PAD_DQWORD(sysv_func->frame.size);
    }
    sysv_func->frame.base.locals = sysv_func->frame.size;
    if (sysv_func->func->locals != NULL) {
        kefir_ir_type_visitor_init(&visitor, frame_local_visitor);
        REQUIRE_OK(kefir_ir_type_visitor_list_nodes(sysv_func->func->locals, &visitor, (void *) sysv_func, 0,
                                                    kefir_ir_type_nodes(sysv_func->func->locals)));
        sysv_func->frame.size = PAD_DQWORD(sysv_func->frame.size);
    }
    return KEFIR_OK;
}

static kefir_result_t function_alloc_return(struct kefir_mem *mem, struct kefir_amd64_sysv_function_decl *sysv_decl) {
    REQUIRE(kefir_ir_type_nodes(sysv_decl->decl->result) <= 1,
            KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Expected IR function to have return type count less than 2"));
    REQUIRE_OK(kefir_amd64_sysv_type_layout(sysv_decl->decl->result, mem, &sysv_decl->returns.layout));
    kefir_result_t res = kefir_amd64_sysv_parameter_classify(mem, sysv_decl->decl->result, &sysv_decl->returns.layout,
                                                             &sysv_decl->returns.allocation);
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

static kefir_result_t function_alloc_params(struct kefir_mem *mem, struct kefir_amd64_sysv_function_decl *sysv_decl) {
    REQUIRE_OK(kefir_amd64_sysv_type_layout(sysv_decl->decl->params, mem, &sysv_decl->parameters.layout));

    kefir_result_t res = kefir_amd64_sysv_parameter_classify(
        mem, sysv_decl->decl->params, &sysv_decl->parameters.layout, &sysv_decl->parameters.allocation);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_vector_free(mem, &sysv_decl->parameters.layout);
        return res;
    });
    sysv_decl->parameters.location = (struct kefir_amd64_sysv_parameter_location){0};
    if (sysv_decl->returns.implicit_parameter) {
        sysv_decl->parameters.location.integer_register++;
    }
    res = kefir_amd64_sysv_parameter_allocate(mem, sysv_decl->decl->params, &sysv_decl->parameters.layout,
                                              &sysv_decl->parameters.allocation, &sysv_decl->parameters.location);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_amd64_sysv_parameter_free(mem, &sysv_decl->parameters.allocation);
        kefir_vector_free(mem, &sysv_decl->parameters.layout);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_function_decl_alloc(struct kefir_mem *mem, const struct kefir_ir_function_decl *decl,
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

static kefir_result_t appendix_removal(struct kefir_mem *mem, struct kefir_hashtree *tree, kefir_hashtree_key_t key,
                                       kefir_hashtree_value_t value, void *payload) {
    UNUSED(tree);
    UNUSED(key);
    UNUSED(payload);
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_function_appendix_data *, data, value);
    if (data->cleanup != NULL) {
        REQUIRE_OK(data->cleanup(mem, data->payload));
    }
    KEFIR_FREE(mem, (char *) key);
    KEFIR_FREE(mem, data);
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_function_alloc(struct kefir_mem *mem,
                                               struct kefir_codegen_amd64_sysv_module *sysv_module,
                                               const struct kefir_ir_function *func,
                                               struct kefir_amd64_sysv_function *sysv_func) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(sysv_module != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected vaaid AMD64 System-V module"));
    REQUIRE(func != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR function"));
    REQUIRE(sysv_func != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AMD64 System-V function"));
    sysv_func->func = func;
    REQUIRE_OK(kefir_amd64_sysv_function_decl_alloc(mem, func->declaration, &sysv_func->decl));
    REQUIRE_OK(kefir_hashtree_init(&sysv_func->appendix, &kefir_hashtree_str_ops));
    REQUIRE_OK(kefir_hashtree_on_removal(&sysv_func->appendix, appendix_removal, NULL));
    if (func->locals != NULL) {
        REQUIRE_OK(kefir_amd64_sysv_type_layout(func->locals, mem, &sysv_func->local_layout));
    }
    kefir_result_t res = calculate_frame(mem, sysv_module, sysv_func);
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
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(sysv_func != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AMD64 System-V function"));
    if (sysv_func->func->locals != NULL) {
        REQUIRE_OK(kefir_vector_free(mem, &sysv_func->local_layout));
    }
    REQUIRE_OK(kefir_hashtree_free(mem, &sysv_func->appendix));
    REQUIRE_OK(kefir_amd64_sysv_function_decl_free(mem, &sysv_func->decl));
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_function_has_appendix(struct kefir_amd64_sysv_function *sysv_func,
                                                      const char *identifier) {
    struct kefir_hashtree_node *node = NULL;
    return kefir_hashtree_at(&sysv_func->appendix, (kefir_hashtree_key_t) identifier, &node);
}

kefir_result_t kefir_amd64_sysv_function_insert_appendix(struct kefir_mem *mem,
                                                         struct kefir_amd64_sysv_function *sysv_func,
                                                         kefir_amd64_sysv_function_appendix_t callback,
                                                         kefir_result_t (*cleanup)(struct kefir_mem *, void *),
                                                         void *payload, const char *identifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(sysv_func != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AMD64 System-V function"));
    REQUIRE(callback != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid appendix callback"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid appendix identifier"));
    kefir_result_t res = kefir_amd64_sysv_function_has_appendix(sysv_func, identifier);
    if (res == KEFIR_NOT_FOUND) {
        char *identifier_copy = KEFIR_MALLOC(mem, strlen(identifier) + 1);
        REQUIRE(identifier_copy != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate appendix identifier copy"));
        strcpy(identifier_copy, identifier);
        struct kefir_amd64_sysv_function_appendix_data *data =
            KEFIR_MALLOC(mem, sizeof(struct kefir_amd64_sysv_function_appendix_data));
        REQUIRE_ELSE(data != NULL, {
            KEFIR_FREE(mem, identifier_copy);
            return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocated appendix data");
        });
        data->callback = callback;
        data->cleanup = cleanup;
        data->payload = payload;
        res = kefir_hashtree_insert(mem, &sysv_func->appendix, (kefir_hashtree_key_t) identifier_copy,
                                    (kefir_hashtree_value_t) data);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_FREE(mem, data);
            KEFIR_FREE(mem, identifier_copy);
            return res;
        });
        return KEFIR_OK;
    } else if (res == KEFIR_OK) {
        return KEFIR_SET_ERROR(KEFIR_ALREADY_EXISTS, "Appendix with specified id already exists");
    } else {
        return res;
    }
}
