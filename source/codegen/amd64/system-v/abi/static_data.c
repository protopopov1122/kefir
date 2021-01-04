#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/codegen/amd64/system-v/abi/data_layout.h"
#include "kefir/codegen/util.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/vector.h"

struct static_data_param {
    struct kefir_codegen_amd64 *codegen;
    const struct kefir_ir_data *data;
    struct kefir_vector data_layout;
    struct kefir_ir_type_visitor *visitor;
    kefir_size_t nested_level;
    kefir_size_t aggregate_offset;
    kefir_size_t slot;
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

static kefir_result_t integer_static_data(const struct kefir_ir_type *type,
                                        kefir_size_t index,
                                        const struct kefir_ir_typeentry *typeentry,
                                        void *payload) {
    UNUSED(type);
    UNUSED(index);
    struct static_data_param *param = (struct static_data_param *) payload;
    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_BOOL:
        case KEFIR_IR_TYPE_CHAR:
        case KEFIR_IR_TYPE_INT8:
            ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_BYTE);
            break;

        case KEFIR_IR_TYPE_SHORT:
        case KEFIR_IR_TYPE_INT16:
            ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_WORD);
            break;

        case KEFIR_IR_TYPE_INT:
        case KEFIR_IR_TYPE_INT32:
            ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_DOUBLE);
            break;

        case KEFIR_IR_TYPE_LONG:
        case KEFIR_IR_TYPE_WORD:
        case KEFIR_IR_TYPE_INT64:
            ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_QUAD);
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Unexpectedly encountered non-integral type");
    }
    ASSIGN_DECL_CAST(struct kefir_ir_data_value *, entry, kefir_vector_at(&param->data->value, param->slot++));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid entry for index"));
    if (entry->undefined) {
        ASMGEN_ARG(&param->codegen->asmgen, KEFIR_INT64_FMT, 0);
    } else {
        ASMGEN_ARG(&param->codegen->asmgen, KEFIR_INT64_FMT, entry->i64);
    }
    return KEFIR_OK;
}

static kefir_result_t float_static_data(const struct kefir_ir_type *type,
                                        kefir_size_t index,
                                        const struct kefir_ir_typeentry *typeentry,
                                        void *payload) {
    UNUSED(type);
    UNUSED(index);
    struct static_data_param *param = (struct static_data_param *) payload;
    ASSIGN_DECL_CAST(struct kefir_ir_data_value *, entry, kefir_vector_at(&param->data->value, param->slot++));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid entry for index"));
    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_FLOAT32:
            ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_DOUBLE);
            if (entry->undefined) {
                ASMGEN_ARG(&param->codegen->asmgen, "%a", 0.0f);
            } else {
                ASMGEN_ARG(&param->codegen->asmgen, "%a", entry->f32[0]);
            }
            break;

        case KEFIR_IR_TYPE_FLOAT64:
            ASMGEN_RAW(&param->codegen->asmgen, KEFIR_AMD64_QUAD);
            if (entry->undefined) {
                ASMGEN_ARG(&param->codegen->asmgen, "%a", 0.0);
            } else {
                ASMGEN_ARG(&param->codegen->asmgen, "%a", entry->f64);
            }
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR,
                KEFIR_AMD64_SYSV_ABI_ERROR_PREFIX "Unexpectedly encountered non-integral type");
    }
    return KEFIR_OK;
}

static kefir_result_t struct_static_data(const struct kefir_ir_type *type,
                                       kefir_size_t index,
                                       const struct kefir_ir_typeentry *typeentry,
                                       void *payload) {
    UNUSED(type);
    struct static_data_param *param = (struct static_data_param *) payload;
    if (param->nested_level == 0) {
        param->aggregate_offset = 0;
    }
    param->nested_level++;
    param->slot++;
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(param->data->type,
                                              param->visitor,
                                              payload,
                                              index + 1,
                                              typeentry->param));
    param->nested_level--;
    return KEFIR_OK;
}

static kefir_result_t align_nested(const struct kefir_ir_type *type,
                                 kefir_size_t index,
                                 const struct kefir_ir_typeentry *typeentry,
                                 void *payload) {
    UNUSED(type);
    UNUSED(typeentry);
    struct static_data_param *param = (struct static_data_param *) payload;
    if (param->nested_level > 0) {
        ASSIGN_DECL_CAST(struct kefir_amd64_sysv_data_layout *, layout,
            kefir_vector_at(&param->data_layout, index));
        const kefir_size_t next_offset = kefir_codegen_pad_aligned(param->aggregate_offset, layout->alignment);
        const kefir_size_t diff = next_offset - param->aggregate_offset;
        param->aggregate_offset = next_offset;
        if (diff > 0) {
            ASMGEN_MULRAW(&param->codegen->asmgen, diff, KEFIR_AMD64_BYTE);
            ASMGEN_ARG0(&param->codegen->asmgen, "0");
        }
        param->aggregate_offset += layout->alignment;
    }
    return KEFIR_OK;
}

kefir_result_t kefir_amd64_sysv_static_data(struct kefir_mem *mem,
                                        struct kefir_codegen_amd64 *codegen,
                                        const struct kefir_ir_data *data) {
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 code generator"));
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data pointer"));
    struct kefir_ir_type_visitor visitor;
    struct static_data_param param = {
        .codegen = codegen,
        .data = data,
        .visitor = &visitor,
        .nested_level = 0,
        .aggregate_offset = 0,
        .slot = 0
    };
    REQUIRE_OK(kefir_amd64_sysv_type_layout(data->type, mem, &param.data_layout));
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, visitor_not_supported));
    KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS(&visitor, integer_static_data);
    KEFIR_IR_TYPE_VISITOR_INIT_FIXED_FP(&visitor, float_static_data);
    visitor.visit[KEFIR_IR_TYPE_STRUCT] = struct_static_data;
    visitor.prehook = align_nested;
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(data->type, &visitor, (void *) &param, 0, kefir_ir_type_nodes(data->type)));
    REQUIRE_OK(kefir_vector_free(mem, &param.data_layout));
    return KEFIR_OK;
}