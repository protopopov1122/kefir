#include "kefir/ir/format.h"
#include "kefir/core/hashtree.h"
#include "kefir/ir/builtins.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/ir/mnemonic.h"
#include "kefir/ir/format_impl.h"
#include "kefir/util/json.h"

kefir_result_t kefir_ir_format_instr_none(struct kefir_json_output *json, const struct kefir_ir_module *module, const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid json output"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "opcode"));
    REQUIRE_OK(kefir_json_output_string(json, kefir_iropcode_mnemonic(instr->opcode)));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_i64(struct kefir_json_output *json, const struct kefir_ir_module *module, const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid json output"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "opcode"));
    REQUIRE_OK(kefir_json_output_string(json, kefir_iropcode_mnemonic(instr->opcode)));
    REQUIRE_OK(kefir_json_output_object_key(json, "arg"));
    REQUIRE_OK(kefir_json_output_integer(json, instr->arg.i64));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_u64(struct kefir_json_output *json, const struct kefir_ir_module *module, const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid json output"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "opcode"));
    REQUIRE_OK(kefir_json_output_string(json, kefir_iropcode_mnemonic(instr->opcode)));
    REQUIRE_OK(kefir_json_output_object_key(json, "arg"));
    REQUIRE_OK(kefir_json_output_uinteger(json, instr->arg.u64));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_f32(struct kefir_json_output *json, const struct kefir_ir_module *module, const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid json output"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));
    
    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "opcode"));
    REQUIRE_OK(kefir_json_output_string(json, kefir_iropcode_mnemonic(instr->opcode)));
    REQUIRE_OK(kefir_json_output_object_key(json, "arg"));
    REQUIRE_OK(kefir_json_output_float(json, instr->arg.f32[0]));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_f64(struct kefir_json_output *json, const struct kefir_ir_module *module, const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid json output"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "opcode"));
    REQUIRE_OK(kefir_json_output_string(json, kefir_iropcode_mnemonic(instr->opcode)));
    REQUIRE_OK(kefir_json_output_object_key(json, "arg"));
    REQUIRE_OK(kefir_json_output_float(json, instr->arg.f64));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_typeref(struct kefir_json_output *json, const struct kefir_ir_module *module, const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid json output"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "opcode"));
    REQUIRE_OK(kefir_json_output_string(json, kefir_iropcode_mnemonic(instr->opcode)));
    REQUIRE_OK(kefir_json_output_object_key(json, "arg"));
    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "type"));
    REQUIRE_OK(kefir_json_output_uinteger(json, instr->arg.u32[0]));
    REQUIRE_OK(kefir_json_output_object_key(json, "index"));
    REQUIRE_OK(kefir_json_output_uinteger(json, instr->arg.u32[1]));
    REQUIRE_OK(kefir_json_output_object_end(json));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_dataref(struct kefir_json_output *json, const struct kefir_ir_module *module, const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid json output"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "opcode"));
    REQUIRE_OK(kefir_json_output_string(json, kefir_iropcode_mnemonic(instr->opcode)));
    REQUIRE_OK(kefir_json_output_object_key(json, "arg"));
    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "data"));
    REQUIRE_OK(kefir_json_output_uinteger(json, instr->arg.u64));
    REQUIRE_OK(kefir_json_output_object_end(json));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_funcref(struct kefir_json_output *json, const struct kefir_ir_module *module, const struct kefir_irinstr *instr) {
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid json output"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR module"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));
    const char *symbol = kefir_ir_module_get_named_symbol(module, (kefir_id_t) instr->arg.u64);
    
    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "opcode"));
    REQUIRE_OK(kefir_json_output_string(json, kefir_iropcode_mnemonic(instr->opcode)));
    REQUIRE_OK(kefir_json_output_object_key(json, "arg"));
    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "function"));
    REQUIRE_OK(kefir_json_output_string(json, symbol));
    REQUIRE_OK(kefir_json_output_object_end(json));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_coderef(struct kefir_json_output *json, const struct kefir_ir_module *module, const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid json output"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "opcode"));
    REQUIRE_OK(kefir_json_output_string(json, kefir_iropcode_mnemonic(instr->opcode)));
    REQUIRE_OK(kefir_json_output_object_key(json, "arg"));
    REQUIRE_OK(kefir_json_output_uinteger(json, instr->arg.u64));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

struct format_param {
    struct kefir_json_output *json;
    struct kefir_ir_type_visitor *visitor;
};

static kefir_result_t format_type_default(const struct kefir_ir_type *type,
                                        kefir_size_t index,
                                        const struct kefir_ir_typeentry *typeentry,
                                        void *payload) {
    UNUSED(type);
    UNUSED(index);
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct format_param *, param,
        payload);
        
    REQUIRE_OK(kefir_json_output_object_key(param->json, "type"));
    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_PAD:
            REQUIRE_OK(kefir_json_output_string(param->json, "pad"));
            REQUIRE_OK(kefir_json_output_object_key(param->json, "length"));
            REQUIRE_OK(kefir_json_output_integer(param->json, typeentry->param));
            break;

        case KEFIR_IR_TYPE_MEMORY:
            REQUIRE_OK(kefir_json_output_string(param->json, "memory"));
            REQUIRE_OK(kefir_json_output_object_key(param->json, "length"));
            REQUIRE_OK(kefir_json_output_integer(param->json, typeentry->param));
            break;

        case KEFIR_IR_TYPE_INT8:
            REQUIRE_OK(kefir_json_output_string(param->json, "int8"));
            break;

        case KEFIR_IR_TYPE_INT16:
            REQUIRE_OK(kefir_json_output_string(param->json, "int16"));
            break;

        case KEFIR_IR_TYPE_INT32:
            REQUIRE_OK(kefir_json_output_string(param->json, "int32"));
            break;

        case KEFIR_IR_TYPE_INT64:
            REQUIRE_OK(kefir_json_output_string(param->json, "int64"));
            break;

        case KEFIR_IR_TYPE_BOOL:
            REQUIRE_OK(kefir_json_output_string(param->json, "bool"));
            break;

        case KEFIR_IR_TYPE_CHAR:
            REQUIRE_OK(kefir_json_output_string(param->json, "char"));
            break;

        case KEFIR_IR_TYPE_SHORT:
            REQUIRE_OK(kefir_json_output_string(param->json, "short"));
            break;

        case KEFIR_IR_TYPE_INT:
            REQUIRE_OK(kefir_json_output_string(param->json, "int"));
            break;

        case KEFIR_IR_TYPE_LONG:
            REQUIRE_OK(kefir_json_output_string(param->json, "long"));
            break;

        case KEFIR_IR_TYPE_FLOAT32:
            REQUIRE_OK(kefir_json_output_string(param->json, "float"));
            break;

        case KEFIR_IR_TYPE_FLOAT64:
            REQUIRE_OK(kefir_json_output_string(param->json, "double"));
            break;

        case KEFIR_IR_TYPE_WORD:
            REQUIRE_OK(kefir_json_output_string(param->json, "word"));
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid type code");
    }
    return KEFIR_OK;
}

static kefir_result_t format_type_struct_union(const struct kefir_ir_type *type,
                                             kefir_size_t index,
                                             const struct kefir_ir_typeentry *typeentry,
                                             void *payload) {
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type"));
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct format_param *, param,
        payload);

    REQUIRE_OK(kefir_json_output_object_key(param->json, "type"));
    REQUIRE_OK(kefir_json_output_string(param->json, "struct"));
    REQUIRE_OK(kefir_json_output_object_key(param->json, "fields"));
    REQUIRE_OK(kefir_json_output_array_begin(param->json));
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(type, param->visitor, payload, index + 1, typeentry->param));
    REQUIRE_OK(kefir_json_output_array_end(param->json));
    return KEFIR_OK;
}

static kefir_result_t format_type_array(const struct kefir_ir_type *type,
                                      kefir_size_t index,
                                      const struct kefir_ir_typeentry *typeentry,
                                      void *payload) {
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type"));
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct format_param *, param,
        payload);

    REQUIRE_OK(kefir_json_output_object_key(param->json, "type"));
    REQUIRE_OK(kefir_json_output_string(param->json, "array"));
    REQUIRE_OK(kefir_json_output_object_key(param->json, "length"));
    REQUIRE_OK(kefir_json_output_integer(param->json, typeentry->param));
    REQUIRE_OK(kefir_json_output_object_key(param->json, "element_type"));
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(type, param->visitor, payload, index + 1, 1));
    return KEFIR_OK;
}

static kefir_result_t format_type_builtin(const struct kefir_ir_type *type,
                                        kefir_size_t index,
                                        const struct kefir_ir_typeentry *typeentry,
                                        void *payload) {
    UNUSED(type);
    UNUSED(index);
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct format_param *, param,
        payload);

    REQUIRE_OK(kefir_json_output_object_key(param->json, "type"));
    REQUIRE_OK(kefir_json_output_string(param->json, "builtin"));
    REQUIRE_OK(kefir_json_output_object_key(param->json, "class"));
    switch (typeentry->param) {
        case KEFIR_IR_TYPE_BUILTIN_VARARG:
            REQUIRE_OK(kefir_json_output_string(param->json, "vararg"));
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid built-in type code");
    }
    return KEFIR_OK;
}

static kefir_result_t format_type_prehook(const struct kefir_ir_type *type,
                                         kefir_size_t index,
                                         const struct kefir_ir_typeentry *typeentry,
                                         void *payload) {
    UNUSED(type);
    UNUSED(index);
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct format_param *, param,
        payload);

    REQUIRE_OK(kefir_json_output_object_begin(param->json));
    return KEFIR_OK;
}

static kefir_result_t format_type_posthook(const struct kefir_ir_type *type,
                                         kefir_size_t index,
                                         const struct kefir_ir_typeentry *typeentry,
                                         void *payload) {
    UNUSED(type);
    UNUSED(index);
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct format_param *, param,
        payload);

    if (typeentry->alignment > 0) {
        REQUIRE_OK(kefir_json_output_object_key(param->json, "alignment"));
        REQUIRE_OK(kefir_json_output_uinteger(param->json, typeentry->alignment));
    }
    REQUIRE_OK(kefir_json_output_object_end(param->json));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_type_json(struct kefir_json_output *json, const struct kefir_ir_type *type) {
    struct kefir_ir_type_visitor visitor;
    REQUIRE_OK(kefir_ir_type_visitor_init(&visitor, format_type_default));
    visitor.visit[KEFIR_IR_TYPE_STRUCT] = format_type_struct_union;
    visitor.visit[KEFIR_IR_TYPE_UNION] = format_type_struct_union;
    visitor.visit[KEFIR_IR_TYPE_ARRAY] = format_type_array;
    visitor.visit[KEFIR_IR_TYPE_BUILTIN] = format_type_builtin;
    visitor.prehook = format_type_prehook;
    visitor.posthook = format_type_posthook;

    struct format_param param = {
        .json = json,
        .visitor = &visitor
    };
    REQUIRE_OK(kefir_json_output_array_begin(json));
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(type, &visitor, &param, 0, kefir_ir_type_total_length(type)));
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_type(FILE *fp, const struct kefir_ir_type *type) {
    REQUIRE(fp != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid file pointer"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type"));

    struct kefir_json_output json;
    REQUIRE_OK(kefir_json_output_init(&json, fp, 4));
    REQUIRE_OK(kefir_ir_format_type_json(&json, type));
    REQUIRE_OK(kefir_json_output_finalize(&json));
    return KEFIR_OK;
}

static kefir_result_t kefir_ir_format_function_declaration(struct kefir_json_output *json, struct kefir_ir_function_decl *decl) {
    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
    REQUIRE_OK(kefir_json_output_string(json, decl->identifier));
    if (decl->alias != NULL) {
        REQUIRE_OK(kefir_json_output_object_key(json, "alias"));
        REQUIRE_OK(kefir_json_output_string(json, decl->alias));
    }
    REQUIRE_OK(kefir_json_output_object_key(json, "parameters"));
    REQUIRE_OK(kefir_ir_format_type_json(json, decl->params));
    REQUIRE_OK(kefir_json_output_object_key(json, "vararg"));
    REQUIRE_OK(kefir_json_output_boolean(json, decl->vararg));
    REQUIRE_OK(kefir_json_output_object_key(json, "returns"));
    REQUIRE_OK(kefir_ir_format_type_json(json, decl->result));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t kefir_ir_format_function(struct kefir_json_output *json,
                                           const struct kefir_ir_module *module,
                                           const struct kefir_ir_function *func)  {
    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
    REQUIRE_OK(kefir_json_output_string(json, func->declaration->identifier));
    
    if (func->locals != NULL) {
        REQUIRE_OK(kefir_json_output_object_key(json, "locals"));
        REQUIRE_OK(kefir_ir_format_type_json(json, func->locals));
    }

    REQUIRE_OK(kefir_json_output_object_key(json, "body"));
    REQUIRE_OK(kefir_json_output_array_begin(json));
    for (kefir_size_t i = 0; i < kefir_irblock_length(&func->body); i++) {
        const struct kefir_irinstr *instr = kefir_irblock_at(&func->body, i);
        REQUIRE_OK(kefir_ir_format_instr(json, module, instr));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));

    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t format_globals(struct kefir_json_output *json, const struct kefir_ir_module *module) {
    REQUIRE_OK(kefir_json_output_array_begin(json));
    const struct kefir_list_entry *entry = NULL;
    for (const char *global = kefir_ir_module_globals_iter(module, &entry);
        global != NULL;
        global = kefir_ir_module_symbol_iter_next(&entry)) {
        REQUIRE_OK(kefir_json_output_string(json, global));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}

static kefir_result_t format_externals(struct kefir_json_output *json, const struct kefir_ir_module *module) {
    REQUIRE_OK(kefir_json_output_array_begin(json));
    const struct kefir_list_entry *entry = NULL;
    for (const char *external = kefir_ir_module_externals_iter(module, &entry);
        external != NULL;
        external = kefir_ir_module_symbol_iter_next(&entry)) {
        REQUIRE_OK(kefir_json_output_string(json, external));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}

static kefir_result_t format_types(struct kefir_json_output *json, const struct kefir_ir_module *module) {
    REQUIRE_OK(kefir_json_output_array_begin(json));
    struct kefir_hashtree_node_iterator iter;
    for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(&module->named_types, &iter);
        node != NULL;
        node = kefir_hashtree_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ir_type *, type,
            node->value);
        REQUIRE_OK(kefir_json_output_object_begin(json));
        REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
        REQUIRE_OK(kefir_json_output_uinteger(json, node->key));
        REQUIRE_OK(kefir_json_output_object_key(json, "type"));
        REQUIRE_OK(kefir_ir_format_type_json(json, type));
        REQUIRE_OK(kefir_json_output_object_end(json));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}

static kefir_result_t format_datum(struct kefir_json_output *json,
                                 const struct kefir_ir_data *data) {
    REQUIRE_OK(kefir_json_output_array_begin(json));
    for (kefir_size_t i = 0; i < kefir_vector_length(&data->value); i++) {
        ASSIGN_DECL_CAST(struct kefir_ir_data_value *, value,
            kefir_vector_at(&data->value, i));
        REQUIRE_OK(kefir_json_output_object_begin(json));
        REQUIRE_OK(kefir_json_output_object_key(json, "type"));
        switch (value->type) {
            case KEFIR_IR_DATA_VALUE_UNDEFINED:
                REQUIRE_OK(kefir_json_output_string(json, "undefined"));
                break;

            case KEFIR_IR_DATA_VALUE_INTEGER:
                REQUIRE_OK(kefir_json_output_string(json, "integer"));
                REQUIRE_OK(kefir_json_output_object_key(json, "value"));
                REQUIRE_OK(kefir_json_output_integer(json, value->value.integer));
                break;

            case KEFIR_IR_DATA_VALUE_FLOAT32:
                REQUIRE_OK(kefir_json_output_string(json, "float32"));
                REQUIRE_OK(kefir_json_output_object_key(json, "value"));
                REQUIRE_OK(kefir_json_output_float(json, value->value.float32));
                break;

            case KEFIR_IR_DATA_VALUE_FLOAT64:
                REQUIRE_OK(kefir_json_output_string(json, "float64"));
                REQUIRE_OK(kefir_json_output_object_key(json, "value"));
                REQUIRE_OK(kefir_json_output_float(json, value->value.float64));
                break;

            case KEFIR_IR_DATA_VALUE_STRING:
                REQUIRE_OK(kefir_json_output_string(json, "string"));
                REQUIRE_OK(kefir_json_output_object_key(json, "value"));
                REQUIRE_OK(kefir_json_output_string(json, value->value.string));
                break;

            case KEFIR_IR_DATA_VALUE_POINTER:
                REQUIRE_OK(kefir_json_output_string(json, "pointer"));
                REQUIRE_OK(kefir_json_output_object_key(json, "reference"));
                REQUIRE_OK(kefir_json_output_string(json, value->value.pointer.reference));
                REQUIRE_OK(kefir_json_output_object_key(json, "offset"));
                REQUIRE_OK(kefir_json_output_integer(json, value->value.pointer.offset));
                break;

            case KEFIR_IR_DATA_VALUE_RAW:
                REQUIRE_OK(kefir_json_output_string(json, "raw"));
                REQUIRE_OK(kefir_json_output_object_key(json, "value"));
                REQUIRE_OK(kefir_json_output_array_begin(json));
                for (kefir_size_t i = 0; i < value->value.raw.length; i++) {
                    REQUIRE_OK(kefir_json_output_integer(json, ((const char *) value->value.raw.data)[i]));
                }
                REQUIRE_OK(kefir_json_output_array_end(json));
                break;

            case KEFIR_IR_DATA_VALUE_AGGREGATE:
                REQUIRE_OK(kefir_json_output_string(json, "aggregate"));
                break;

        }
        REQUIRE_OK(kefir_json_output_object_end(json));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}

static kefir_result_t format_data(struct kefir_json_output *json, const struct kefir_ir_module *module) {
    REQUIRE_OK(kefir_json_output_array_begin(json));
    struct kefir_hashtree_node_iterator iter;
    const char *identifier;
    for (const struct kefir_ir_data *data = kefir_ir_module_named_data_iter(module, &iter, &identifier);
        data != NULL;
        data = kefir_ir_module_named_data_next(&iter, &identifier)) {
        
        REQUIRE_OK(kefir_json_output_object_begin(json));
        REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
        REQUIRE_OK(kefir_json_output_string(json, identifier));
        REQUIRE_OK(kefir_json_output_object_key(json, "type"));
        REQUIRE_OK(kefir_ir_format_type_json(json, data->type));
        REQUIRE_OK(kefir_json_output_object_key(json, "value"));
        REQUIRE_OK(format_datum(json, data));
        REQUIRE_OK(kefir_json_output_object_end(json));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}

static kefir_result_t format_function_declarations(struct kefir_json_output *json, const struct kefir_ir_module *module) {
    REQUIRE_OK(kefir_json_output_array_begin(json));
    struct kefir_hashtree_node_iterator iter;
    for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(&module->function_declarations, &iter);
        node != NULL;
        node = kefir_hashtree_next(&iter)) {
        REQUIRE_OK(kefir_ir_format_function_declaration(json, (struct kefir_ir_function_decl *) node->value));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}

static kefir_result_t format_functions(struct kefir_json_output *json, const struct kefir_ir_module *module) {
    REQUIRE_OK(kefir_json_output_array_begin(json));
    struct kefir_hashtree_node_iterator iter;
    for (const struct kefir_ir_function *func = kefir_ir_module_function_iter(module, &iter);
        func != NULL;
        func = kefir_ir_module_function_next(&iter)) {
        REQUIRE_OK(kefir_ir_format_function(json, module, func));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_module_json(struct kefir_json_output *json, const struct kefir_ir_module *module) {
    REQUIRE_OK(kefir_json_output_object_begin(json));

    REQUIRE_OK(kefir_json_output_object_key(json, "globals"));
    REQUIRE_OK(format_globals(json, module));
    REQUIRE_OK(kefir_json_output_object_key(json, "externals"));
    REQUIRE_OK(format_externals(json, module));
    REQUIRE_OK(kefir_json_output_object_key(json, "types"));
    REQUIRE_OK(format_types(json, module));
    REQUIRE_OK(kefir_json_output_object_key(json, "data"));
    REQUIRE_OK(format_data(json, module));
    REQUIRE_OK(kefir_json_output_object_key(json, "function_declarations"));
    REQUIRE_OK(format_function_declarations(json, module));
    REQUIRE_OK(kefir_json_output_object_key(json, "functions"));
    REQUIRE_OK(format_functions(json, module));    

    REQUIRE_OK(kefir_json_output_object_end(json));
    REQUIRE_OK(kefir_json_output_finalize(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_module(FILE *fp, const struct kefir_ir_module *module) {
    struct kefir_json_output json;
    REQUIRE_OK(kefir_json_output_init(&json, fp, 4));
    REQUIRE_OK(kefir_ir_format_module_json(&json, module));
    REQUIRE_OK(kefir_json_output_finalize(&json));
    return KEFIR_OK;
}