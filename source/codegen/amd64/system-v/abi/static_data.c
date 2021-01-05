#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/amd64/system-v/abi/data_layout.h"
#include "kefir/codegen/util.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/vector.h"

#ifndef __STDC_IEC_559__
#error "Expected host architecture to conform with IEEE-754"
#endif

struct static_data_param {
    struct kefir_codegen_amd64 *codegen;
    const struct kefir_ir_data *data;
    struct kefir_vector layout;
    struct kefir_ir_type_visitor *visitor;
    kefir_size_t slot;
    unsigned char *buffer;
};

static kefir_result_t visitor_not_supported(const struct kefir_ir_type *type,
                                          kefir_size_t index,
                                          const struct kefir_ir_typeentry *typeentry,
                                          void *payload) {
    UNUSED(type);
    UNUSED(index);
    UNUSED(typeentry);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, "Encountered not supported type code while traversing type");
}

static kefir_result_t scalar_static_data(const struct kefir_ir_type *type,
                                        kefir_size_t index,
                                        const struct kefir_ir_typeentry *typeentry,
                                        void *payload) {
    UNUSED(type);
    struct static_data_param *param = (struct static_data_param *) payload;
    ASSIGN_DECL_CAST(struct kefir_ir_data_value *, entry, kefir_vector_at(&param->data->value, param->slot++));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid entry for index"));
    REQUIRE(!entry->undefined, KEFIR_OK);
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout,
        kefir_vector_at(&param->layout, index));
    kefir_int64_t value = entry->i64;
    kefir_size_t bytes = 0;
    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_BOOL:
        case KEFIR_IR_TYPE_CHAR:
        case KEFIR_IR_TYPE_INT8:
            bytes = 1;
            break;

        case KEFIR_IR_TYPE_SHORT:
        case KEFIR_IR_TYPE_INT16:
            bytes = 2;
            break;

        case KEFIR_IR_TYPE_INT:
        case KEFIR_IR_TYPE_INT32:
        case KEFIR_IR_TYPE_FLOAT32:
            bytes = 4;
            break;

        case KEFIR_IR_TYPE_LONG:
        case KEFIR_IR_TYPE_WORD:
        case KEFIR_IR_TYPE_INT64:
        case KEFIR_IR_TYPE_FLOAT64:
            bytes = 8;
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Unexpectedly encountered non-integral type");
    }
    for (kefir_size_t i = 0; i < bytes; i++) {
        param->buffer[layout->relative_offset + i] = (unsigned char) ((value >> (i << 3)) & 0xff);
    }
    return KEFIR_OK;
}

static kefir_result_t struct_union_static_data(const struct kefir_ir_type *type,
                                       kefir_size_t index,
                                       const struct kefir_ir_typeentry *typeentry,
                                       void *payload) {
    UNUSED(type);
    struct static_data_param *param = (struct static_data_param *) payload;
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout,
        kefir_vector_at(&param->layout, index));
    unsigned char *outer_buffer = param->buffer;
    param->buffer += layout->relative_offset;
    param->slot++;
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(param->data->type,
                                              param->visitor,
                                              payload,
                                              index + 1,
                                              typeentry->param));
    param->buffer = outer_buffer;
    return KEFIR_OK;
}

static kefir_result_t array_static_data(const struct kefir_ir_type *type,
                                       kefir_size_t index,
                                       const struct kefir_ir_typeentry *typeentry,
                                       void *payload) {
    UNUSED(type);
    struct static_data_param *param = (struct static_data_param *) payload;
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout,
        kefir_vector_at(&param->layout, index));
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, element_layout,
        kefir_vector_at(&param->layout, index + 1));
    unsigned char *outer_buffer = param->buffer;
    param->buffer += layout->relative_offset;
    param->slot++;
    unsigned char *buffer_base  = param->buffer;
    kefir_size_t offset = 0;
    for (kefir_size_t i = 0; i < (kefir_size_t) typeentry->param; i++) {
        offset = kefir_codegen_pad_aligned(offset, element_layout->alignment);
        param->buffer = buffer_base + offset;
        REQUIRE_OK(kefir_ir_type_visitor_list_nodes(param->data->type,
                                                param->visitor,
                                                payload,
                                                index + 1,
                                                1));
        offset += element_layout->size;
        param->buffer = buffer_base + offset;
    }
    param->buffer = outer_buffer;
    return KEFIR_OK;
}

struct total_size {
    kefir_size_t size;
    kefir_size_t alignment;
    struct kefir_vector *layout;
};

static kefir_result_t calculate_total_size(const struct kefir_ir_type *type,
                                         kefir_size_t index,
                                         const struct kefir_ir_typeentry *typeentry,
                                         void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    struct total_size *size = (struct total_size *) payload;
    ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout,
        kefir_vector_at(size->layout, index));
    size->size = kefir_codegen_pad_aligned(size->size, layout->alignment);
    size->size += layout->size;
    size->alignment = MAX(size->alignment, layout->alignment);
    return KEFIR_OK;
}

static kefir_result_t type_total_size(const struct kefir_ir_type *type,
                                    struct kefir_vector *layout,
                                    kefir_size_t *size,
                                    kefir_size_t *alignment) {
    struct total_size total_size = {
        .size = 0,
        .alignment = 0,
        .layout = layout
    };
    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, calculate_total_size));
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(type, &visitor, (void *) &total_size, 0, kefir_ir_type_nodes(type)));
    *size = total_size.size;
    *alignment = total_size.alignment;
    return KEFIR_OK;
}

static kefir_result_t dump_buffer(struct kefir_codegen_amd64 *codegen,
                                const unsigned char *buffer,
                                kefir_size_t size,
                                kefir_size_t alignment,
                                const char *identifier) {
    if (identifier != NULL) {
        if (alignment > 1) {
            ASMGEN_INSTR(&codegen->asmgen, KEFIR_AMD64_ALIGN);
            ASMGEN_ARG(&codegen->asmgen, KEFIR_SIZE_FMT, alignment);
        }
        ASMGEN_LABEL(&codegen->asmgen, "%s", identifier);
    }
    for (kefir_size_t i = 0; i < size; i++) {
        if (i % KEFIR_AMD64_SYSV_ABI_QWORD == 0) {
            ASMGEN_RAW(&codegen->asmgen, KEFIR_AMD64_BYTE);
        }
        ASMGEN_ARG(&codegen->asmgen, "0x%02x", buffer[i]);
    }
    ASMGEN_NEWLINE(&codegen->asmgen, 1);
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_static_data(struct kefir_mem *mem,
                                        struct kefir_codegen_amd64 *codegen,
                                        const struct kefir_ir_data *data,
                                        const char *identifier) {
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 code generator"));
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data pointer"));
    struct kefir_ir_type_visitor visitor;
    struct static_data_param param = {
        .codegen = codegen,
        .data = data,
        .visitor = &visitor,
        .slot = 0
    };
    REQUIRE_OK(kefir_amd64_sysv_type_layout(data->type, mem, &param.layout));
    kefir_size_t buffer_size = 0, buffer_alignment = 0;
    kefir_result_t res = type_total_size(data->type, &param.layout, &buffer_size, &buffer_alignment);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_vector_free(mem, &param.layout);
        return res;
    });
    param.buffer = KEFIR_CALLOC(mem, 1, buffer_size);
    REQUIRE_ELSE(param.buffer != NULL, {
        kefir_vector_free(mem, &param.layout);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate a buffer for static data");
    });
    res = kefir_ir_type_visitor_init(&visitor, visitor_not_supported);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, param.buffer);
        kefir_vector_free(mem, &param.layout);
        return res;
    });
    KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS(&visitor, scalar_static_data);
    KEFIR_IR_TYPE_VISITOR_INIT_FIXED_FP(&visitor, scalar_static_data);
    visitor.visit[KEFIR_IR_TYPE_STRUCT] = struct_union_static_data;
    visitor.visit[KEFIR_IR_TYPE_UNION] = struct_union_static_data;
    visitor.visit[KEFIR_IR_TYPE_ARRAY] = array_static_data;
    res = kefir_ir_type_visitor_list_nodes(data->type, &visitor, (void *) &param, 0, kefir_ir_type_nodes(data->type));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, param.buffer);
        kefir_vector_free(mem, &param.layout);
        return res;
    });
    res = dump_buffer(codegen, param.buffer, buffer_size, buffer_alignment, identifier);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, param.buffer);
        kefir_vector_free(mem, &param.layout);
        return res;
    });
    KEFIR_FREE(mem, param.buffer);
    REQUIRE_OK(kefir_vector_free(mem, &param.layout));
    return KEFIR_OK;
}