#include "kefir/codegen/amd64/system-v/abi.h"
#include "kefir/codegen/amd64/shortcuts.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

struct static_data_param {
    struct kefir_codegen_amd64 *codegen;
    const struct kefir_ir_data *data;
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
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    ASSIGN_DECL_CAST(struct kefir_ir_data_value *, entry, kefir_vector_at(&param->data->value, iter.slot));
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
    struct static_data_param *param = (struct static_data_param *) payload;
    struct kefir_ir_type_iterator iter;
    REQUIRE_OK(kefir_ir_type_iterator_init(type, &iter));
    REQUIRE_OK(kefir_ir_type_iterator_goto(&iter, index));
    ASSIGN_DECL_CAST(struct kefir_ir_data_value *, entry, kefir_vector_at(&param->data->value, iter.slot));
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


kefir_result_t kefir_amd64_sysv_static_data(struct kefir_codegen_amd64 *codegen,
                                        const struct kefir_ir_data *data) {
    REQUIRE(codegen != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AMD64 code generator"));
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data pointer"));
    struct static_data_param param = {
        .codegen = codegen,
        .data = data
    };
    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, visitor_not_supported));
    KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS(&visitor, integer_static_data);
    KEFIR_IR_TYPE_VISITOR_INIT_FIXED_FP(&visitor, float_static_data);
    return kefir_ir_type_visitor_list_nodes(data->type, &visitor, (void *) &param, 0, kefir_ir_type_nodes(data->type));
}