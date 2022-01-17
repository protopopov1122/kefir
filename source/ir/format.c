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

#include "kefir/ir/format.h"
#include "kefir/core/hashtree.h"
#include "kefir/ir/builtins.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/ir/mnemonic.h"
#include "kefir/ir/format_impl.h"
#include "kefir/util/json.h"

kefir_result_t kefir_ir_format_instr_none(struct kefir_json_output *json, const struct kefir_ir_module *module,
                                          const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid json output"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "opcode"));
    REQUIRE_OK(kefir_json_output_string(json, kefir_iropcode_mnemonic(instr->opcode)));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_i64(struct kefir_json_output *json, const struct kefir_ir_module *module,
                                         const struct kefir_irinstr *instr) {
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

kefir_result_t kefir_ir_format_instr_u64(struct kefir_json_output *json, const struct kefir_ir_module *module,
                                         const struct kefir_irinstr *instr) {
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

kefir_result_t kefir_ir_format_instr_bool(struct kefir_json_output *json, const struct kefir_ir_module *module,
                                          const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid json output"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "opcode"));
    REQUIRE_OK(kefir_json_output_string(json, kefir_iropcode_mnemonic(instr->opcode)));
    REQUIRE_OK(kefir_json_output_object_key(json, "arg"));
    REQUIRE_OK(kefir_json_output_boolean(json, instr->arg.u64 != 0));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_u32(struct kefir_json_output *json, const struct kefir_ir_module *module,
                                         const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid json output"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "opcode"));
    REQUIRE_OK(kefir_json_output_string(json, kefir_iropcode_mnemonic(instr->opcode)));
    REQUIRE_OK(kefir_json_output_object_key(json, "arg"));
    REQUIRE_OK(kefir_json_output_array_begin(json));
    REQUIRE_OK(kefir_json_output_uinteger(json, instr->arg.u32[0]));
    REQUIRE_OK(kefir_json_output_uinteger(json, instr->arg.u32[1]));
    REQUIRE_OK(kefir_json_output_array_end(json));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_f32(struct kefir_json_output *json, const struct kefir_ir_module *module,
                                         const struct kefir_irinstr *instr) {
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

kefir_result_t kefir_ir_format_instr_f64(struct kefir_json_output *json, const struct kefir_ir_module *module,
                                         const struct kefir_irinstr *instr) {
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

kefir_result_t kefir_ir_format_instr_typeref(struct kefir_json_output *json, const struct kefir_ir_module *module,
                                             const struct kefir_irinstr *instr) {
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

kefir_result_t kefir_ir_format_instr_identifier(struct kefir_json_output *json, const struct kefir_ir_module *module,
                                                const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid json output"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "opcode"));
    REQUIRE_OK(kefir_json_output_string(json, kefir_iropcode_mnemonic(instr->opcode)));
    REQUIRE_OK(kefir_json_output_object_key(json, "arg"));
    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "data"));
    REQUIRE_OK(kefir_json_output_string(json, kefir_symbol_table_get(&module->symbols, instr->arg.u64)));
    REQUIRE_OK(kefir_json_output_object_end(json));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_funcref(struct kefir_json_output *json, const struct kefir_ir_module *module,
                                             const struct kefir_irinstr *instr) {
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid json output"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR module"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));
    const struct kefir_ir_function_decl *decl = kefir_ir_module_get_declaration(module, (kefir_id_t) instr->arg.u64);

    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "opcode"));
    REQUIRE_OK(kefir_json_output_string(json, kefir_iropcode_mnemonic(instr->opcode)));
    REQUIRE_OK(kefir_json_output_object_key(json, "arg"));
    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
    REQUIRE_OK(kefir_json_output_uinteger(json, decl->id));
    REQUIRE_OK(kefir_json_output_object_key(json, "name"));
    if (decl->name != NULL) {
        REQUIRE_OK(kefir_json_output_string(json, decl->name));
    } else {
        REQUIRE_OK(kefir_json_output_null(json));
    }
    REQUIRE_OK(kefir_json_output_object_end(json));
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_coderef(struct kefir_json_output *json, const struct kefir_ir_module *module,
                                             const struct kefir_irinstr *instr) {
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

kefir_result_t kefir_ir_format_instr_string(struct kefir_json_output *json, const struct kefir_ir_module *module,
                                            const struct kefir_irinstr *instr) {
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

struct format_param {
    struct kefir_json_output *json;
    struct kefir_ir_type_visitor *visitor;
};

static const char *typecode_to_string(kefir_ir_typecode_t typecode) {
    switch (typecode) {
        case KEFIR_IR_TYPE_PAD:
            return "pad";

        case KEFIR_IR_TYPE_STRUCT:
            return "struct";

        case KEFIR_IR_TYPE_ARRAY:
            return "array";

        case KEFIR_IR_TYPE_UNION:
            return "union";

        case KEFIR_IR_TYPE_MEMORY:
            return "memory";

        case KEFIR_IR_TYPE_INT8:
            return "int8";

        case KEFIR_IR_TYPE_INT16:
            return "int16";

        case KEFIR_IR_TYPE_INT32:
            return "int32";

        case KEFIR_IR_TYPE_INT64:
            return "int64";

        case KEFIR_IR_TYPE_FLOAT32:
            return "float";

        case KEFIR_IR_TYPE_FLOAT64:
            return "double";

        case KEFIR_IR_TYPE_LONG_DOUBLE:
            return "long_double";

        case KEFIR_IR_TYPE_BOOL:
            return "bool";

        case KEFIR_IR_TYPE_CHAR:
            return "char";

        case KEFIR_IR_TYPE_SHORT:
            return "short";

        case KEFIR_IR_TYPE_INT:
            return "int";

        case KEFIR_IR_TYPE_LONG:
            return "long";

        case KEFIR_IR_TYPE_WORD:
            return "word";

        case KEFIR_IR_TYPE_BITS:
            return "bits";

        case KEFIR_IR_TYPE_BUILTIN:
            return "builtin";

        default:
            return NULL;
    }
}

static kefir_result_t format_type_default(const struct kefir_ir_type *type, kefir_size_t index,
                                          const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct format_param *, param, payload);

    REQUIRE_OK(kefir_json_output_object_key(param->json, "type"));

    const char *type_literal = typecode_to_string(typeentry->typecode);
    REQUIRE(type_literal != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unknown IR type code"));
    REQUIRE_OK(kefir_json_output_string(param->json, type_literal));

    switch (typeentry->typecode) {
        case KEFIR_IR_TYPE_PAD:
            REQUIRE_OK(kefir_json_output_object_key(param->json, "length"));
            REQUIRE_OK(kefir_json_output_integer(param->json, typeentry->param));
            break;

        case KEFIR_IR_TYPE_MEMORY:
            REQUIRE_OK(kefir_json_output_object_key(param->json, "length"));
            REQUIRE_OK(kefir_json_output_integer(param->json, typeentry->param));
            break;

        case KEFIR_IR_TYPE_INT8:
        case KEFIR_IR_TYPE_INT16:
        case KEFIR_IR_TYPE_INT32:
        case KEFIR_IR_TYPE_INT64:
        case KEFIR_IR_TYPE_BOOL:
        case KEFIR_IR_TYPE_CHAR:
        case KEFIR_IR_TYPE_SHORT:
        case KEFIR_IR_TYPE_INT:
        case KEFIR_IR_TYPE_LONG:
        case KEFIR_IR_TYPE_FLOAT32:
        case KEFIR_IR_TYPE_FLOAT64:
        case KEFIR_IR_TYPE_LONG_DOUBLE:
        case KEFIR_IR_TYPE_WORD:
            break;

        case KEFIR_IR_TYPE_BITS: {
            kefir_size_t bits = typeentry->param;

            REQUIRE_OK(kefir_json_output_object_key(param->json, "width"));
            REQUIRE_OK(kefir_json_output_integer(param->json, bits));
        } break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid type code");
    }
    return KEFIR_OK;
}

static kefir_result_t format_type_struct_union(const struct kefir_ir_type *type, kefir_size_t index,
                                               const struct kefir_ir_typeentry *typeentry, void *payload) {
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type"));
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct format_param *, param, payload);

    REQUIRE_OK(kefir_json_output_object_key(param->json, "type"));
    REQUIRE_OK(kefir_json_output_string(param->json, typeentry->typecode == KEFIR_IR_TYPE_STRUCT ? "struct" : "union"));
    REQUIRE_OK(kefir_json_output_object_key(param->json, "fields"));
    REQUIRE_OK(kefir_json_output_array_begin(param->json));
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(type, param->visitor, payload, index + 1, typeentry->param));
    REQUIRE_OK(kefir_json_output_array_end(param->json));
    return KEFIR_OK;
}

static kefir_result_t format_type_array(const struct kefir_ir_type *type, kefir_size_t index,
                                        const struct kefir_ir_typeentry *typeentry, void *payload) {
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type"));
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct format_param *, param, payload);

    REQUIRE_OK(kefir_json_output_object_key(param->json, "type"));
    REQUIRE_OK(kefir_json_output_string(param->json, "array"));
    REQUIRE_OK(kefir_json_output_object_key(param->json, "length"));
    REQUIRE_OK(kefir_json_output_integer(param->json, typeentry->param));
    REQUIRE_OK(kefir_json_output_object_key(param->json, "element_type"));
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(type, param->visitor, payload, index + 1, 1));
    return KEFIR_OK;
}

static kefir_result_t format_type_builtin(const struct kefir_ir_type *type, kefir_size_t index,
                                          const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct format_param *, param, payload);

    REQUIRE_OK(kefir_json_output_object_key(param->json, "type"));
    REQUIRE_OK(kefir_json_output_string(param->json, "builtin"));
    REQUIRE_OK(kefir_json_output_object_key(param->json, "class"));
    switch (typeentry->param) {
        case KEFIR_IR_TYPE_BUILTIN_VARARG:
            REQUIRE_OK(kefir_json_output_string(param->json, "vararg"));
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid built-in type code");
    }
    return KEFIR_OK;
}

static kefir_result_t format_type_prehook(const struct kefir_ir_type *type, kefir_size_t index,
                                          const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct format_param *, param, payload);

    REQUIRE_OK(kefir_json_output_object_begin(param->json));
    return KEFIR_OK;
}

static kefir_result_t format_type_posthook(const struct kefir_ir_type *type, kefir_size_t index,
                                           const struct kefir_ir_typeentry *typeentry, void *payload) {
    UNUSED(type);
    UNUSED(index);
    REQUIRE(typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR type entry"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct format_param *, param, payload);

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

    struct format_param param = {.json = json, .visitor = &visitor};
    REQUIRE_OK(kefir_json_output_array_begin(json));
    REQUIRE_OK(kefir_ir_type_visitor_list_nodes(type, &visitor, &param, 0, kefir_ir_type_total_length(type)));
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_type(FILE *fp, const struct kefir_ir_type *type) {
    REQUIRE(fp != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid file pointer"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR type"));

    struct kefir_json_output json;
    REQUIRE_OK(kefir_json_output_init(&json, fp, 4));
    REQUIRE_OK(kefir_ir_format_type_json(&json, type));
    REQUIRE_OK(kefir_json_output_finalize(&json));
    return KEFIR_OK;
}

static kefir_result_t kefir_ir_format_function_declaration(struct kefir_json_output *json,
                                                           struct kefir_ir_function_decl *decl) {
    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
    REQUIRE_OK(kefir_json_output_uinteger(json, decl->id));
    REQUIRE_OK(kefir_json_output_object_key(json, "name"));
    if (decl->name != NULL) {
        REQUIRE_OK(kefir_json_output_string(json, decl->name));
    } else {
        REQUIRE_OK(kefir_json_output_null(json));
    }
    REQUIRE_OK(kefir_json_output_object_key(json, "parameters"));
    if (decl->params_type_id != KEFIR_ID_NONE) {
        REQUIRE_OK(kefir_json_output_uinteger(json, decl->params_type_id));
    } else {
        REQUIRE_OK(kefir_ir_format_type_json(json, decl->params));
    }
    REQUIRE_OK(kefir_json_output_object_key(json, "vararg"));
    REQUIRE_OK(kefir_json_output_boolean(json, decl->vararg));
    REQUIRE_OK(kefir_json_output_object_key(json, "returns"));
    if (decl->result_type_id != KEFIR_ID_NONE) {
        REQUIRE_OK(kefir_json_output_uinteger(json, decl->result_type_id));
    } else {
        REQUIRE_OK(kefir_ir_format_type_json(json, decl->result));
    }
    REQUIRE_OK(kefir_json_output_object_end(json));
    return KEFIR_OK;
}

static kefir_result_t kefir_ir_format_function(struct kefir_json_output *json, const struct kefir_ir_module *module,
                                               const struct kefir_ir_function *func) {
    REQUIRE_OK(kefir_json_output_object_begin(json));
    REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
    REQUIRE_OK(kefir_json_output_uinteger(json, func->declaration->id));
    REQUIRE_OK(kefir_json_output_object_key(json, "name"));
    REQUIRE_OK(kefir_json_output_string(json, func->name));

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
    for (const char *global = kefir_ir_module_globals_iter(module, &entry); global != NULL;
         global = kefir_ir_module_globals_iter_next(&entry)) {
        REQUIRE_OK(kefir_json_output_string(json, global));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}

static kefir_result_t format_externals(struct kefir_json_output *json, const struct kefir_ir_module *module) {
    REQUIRE_OK(kefir_json_output_array_begin(json));
    struct kefir_hashtree_node_iterator externals_iter;
    for (const char *external = kefir_ir_module_externals_iter(module, &externals_iter); external != NULL;
         external = kefir_ir_module_externals_iter_next(&externals_iter)) {
        REQUIRE_OK(kefir_json_output_string(json, external));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}

static kefir_result_t format_types(struct kefir_json_output *json, const struct kefir_ir_module *module) {
    REQUIRE_OK(kefir_json_output_array_begin(json));
    struct kefir_hashtree_node_iterator iter;
    for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(&module->named_types, &iter); node != NULL;
         node = kefir_hashtree_next(&iter)) {
        ASSIGN_DECL_CAST(struct kefir_ir_type *, type, node->value);
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

static kefir_result_t format_datum(struct kefir_json_output *json, const struct kefir_ir_data *data) {
    REQUIRE_OK(kefir_json_output_array_begin(json));
    kefir_size_t undefined_count = 0;
    for (kefir_size_t i = 0; i < kefir_vector_length(&data->value); i++) {
        ASSIGN_DECL_CAST(struct kefir_ir_data_value *, value, kefir_vector_at(&data->value, i));

        if (value->type != KEFIR_IR_DATA_VALUE_UNDEFINED) {
            if (undefined_count > 0) {
                REQUIRE_OK(kefir_json_output_object_begin(json));
                REQUIRE_OK(kefir_json_output_object_key(json, "class"));
                REQUIRE_OK(kefir_json_output_string(json, "undefined"));
                if (undefined_count > 1) {
                    REQUIRE_OK(kefir_json_output_object_key(json, "count"));
                    REQUIRE_OK(kefir_json_output_uinteger(json, undefined_count));
                }
                REQUIRE_OK(kefir_json_output_object_end(json));
                undefined_count = 0;
            }
            REQUIRE_OK(kefir_json_output_object_begin(json));
            REQUIRE_OK(kefir_json_output_object_key(json, "class"));
        }

        switch (value->type) {
            case KEFIR_IR_DATA_VALUE_UNDEFINED:
                undefined_count++;
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

            case KEFIR_IR_DATA_VALUE_LONG_DOUBLE:
                REQUIRE_OK(kefir_json_output_string(json, "long_double"));
                REQUIRE_OK(kefir_json_output_object_key(json, "value"));
                REQUIRE_OK(kefir_json_output_long_double(json, value->value.long_double));
                break;

            case KEFIR_IR_DATA_VALUE_STRING:
                REQUIRE_OK(kefir_json_output_string(json, "string"));
                REQUIRE_OK(kefir_json_output_object_key(json, "content"));
                REQUIRE_OK(
                    kefir_json_output_raw_string(json, (const char *) value->value.raw.data, value->value.raw.length));
                REQUIRE_OK(kefir_json_output_object_key(json, "length"));
                REQUIRE_OK(kefir_json_output_uinteger(json, value->value.raw.length));
                break;

            case KEFIR_IR_DATA_VALUE_POINTER:
                REQUIRE_OK(kefir_json_output_string(json, "pointer"));
                REQUIRE_OK(kefir_json_output_object_key(json, "reference"));
                REQUIRE_OK(kefir_json_output_string(json, value->value.pointer.reference));
                REQUIRE_OK(kefir_json_output_object_key(json, "offset"));
                REQUIRE_OK(kefir_json_output_integer(json, value->value.pointer.offset));
                break;

            case KEFIR_IR_DATA_VALUE_STRING_POINTER: {
                REQUIRE_OK(kefir_json_output_string(json, "string_pointer"));
                REQUIRE_OK(kefir_json_output_object_key(json, "string"));
                REQUIRE_OK(kefir_json_output_integer(json, value->value.string_ptr.id));
                REQUIRE_OK(kefir_json_output_object_key(json, "offset"));
                REQUIRE_OK(kefir_json_output_integer(json, value->value.pointer.offset));
            } break;

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
        if (value->type != KEFIR_IR_DATA_VALUE_UNDEFINED) {
            REQUIRE_OK(kefir_json_output_object_end(json));
        }
    }

    if (undefined_count > 0) {
        REQUIRE_OK(kefir_json_output_object_begin(json));
        REQUIRE_OK(kefir_json_output_object_key(json, "class"));
        REQUIRE_OK(kefir_json_output_string(json, "undefined"));
        if (undefined_count > 1) {
            REQUIRE_OK(kefir_json_output_object_key(json, "count"));
            REQUIRE_OK(kefir_json_output_uinteger(json, undefined_count));
        }
        REQUIRE_OK(kefir_json_output_object_end(json));
        undefined_count = 0;
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}

static kefir_result_t format_data(struct kefir_json_output *json, const struct kefir_ir_module *module) {
    REQUIRE_OK(kefir_json_output_array_begin(json));
    struct kefir_hashtree_node_iterator iter;
    const char *identifier;
    for (const struct kefir_ir_data *data = kefir_ir_module_named_data_iter(module, &iter, &identifier); data != NULL;
         data = kefir_ir_module_named_data_next(&iter, &identifier)) {

        REQUIRE_OK(kefir_json_output_object_begin(json));
        REQUIRE_OK(kefir_json_output_object_key(json, "identifier"));
        REQUIRE_OK(kefir_json_output_string(json, identifier));
        REQUIRE_OK(kefir_json_output_object_key(json, "storage"));
        switch (data->storage) {
            case KEFIR_IR_DATA_GLOBAL_STORAGE:
                REQUIRE_OK(kefir_json_output_string(json, "global"));
                break;

            case KEFIR_IR_DATA_THREAD_LOCAL_STORAGE:
                REQUIRE_OK(kefir_json_output_string(json, "thread_local"));
                break;
        }
        REQUIRE_OK(kefir_json_output_object_key(json, "type"));
        REQUIRE_OK(kefir_json_output_uinteger(json, data->type_id));
        REQUIRE_OK(kefir_json_output_object_key(json, "value"));
        REQUIRE_OK(format_datum(json, data));
        REQUIRE_OK(kefir_json_output_object_end(json));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}

static kefir_result_t format_string_literal(struct kefir_json_output *json, const struct kefir_ir_module *module) {
    REQUIRE_OK(kefir_json_output_array_begin(json));
    struct kefir_hashtree_node_iterator iter;
    kefir_id_t id;
    kefir_ir_string_literal_type_t literal_type;
    kefir_bool_t public;
    const void *literal;
    kefir_size_t length;
    kefir_result_t res = KEFIR_OK;
    for (res = kefir_ir_module_string_literal_iter(module, &iter, &id, &literal_type, &public, &literal, &length);
         res == KEFIR_OK;
         res = kefir_ir_module_string_literal_next(&iter, &id, &literal_type, &public, &literal, &length)) {
        REQUIRE_OK(kefir_json_output_object_begin(json));
        REQUIRE_OK(kefir_json_output_object_key(json, "id"));
        REQUIRE_OK(kefir_json_output_integer(json, id));
        REQUIRE_OK(kefir_json_output_object_key(json, "public"));
        REQUIRE_OK(kefir_json_output_boolean(json, public));
        REQUIRE_OK(kefir_json_output_object_key(json, "type"));
        switch (literal_type) {
            case KEFIR_IR_STRING_LITERAL_MULTIBYTE:
                REQUIRE_OK(kefir_json_output_string(json, "multibyte"));
                REQUIRE_OK(kefir_json_output_object_key(json, "literal"));
                REQUIRE_OK(kefir_json_output_raw_string(json, literal, length));
                break;

            case KEFIR_IR_STRING_LITERAL_UNICODE16:
                REQUIRE_OK(kefir_json_output_string(json, "unicode16"));
                REQUIRE_OK(kefir_json_output_object_key(json, "literal"));
                REQUIRE_OK(kefir_json_output_array_begin(json));
                for (kefir_size_t i = 0; i < length; i++) {
                    kefir_char16_t chr = ((kefir_char16_t *) literal)[i];
                    REQUIRE_OK(kefir_json_output_uinteger(json, chr));
                }
                REQUIRE_OK(kefir_json_output_array_end(json));
                break;

            case KEFIR_IR_STRING_LITERAL_UNICODE32:
                REQUIRE_OK(kefir_json_output_string(json, "unicode32"));
                REQUIRE_OK(kefir_json_output_object_key(json, "literal"));
                REQUIRE_OK(kefir_json_output_array_begin(json));
                for (kefir_size_t i = 0; i < length; i++) {
                    kefir_char32_t chr = ((kefir_char32_t *) literal)[i];
                    REQUIRE_OK(kefir_json_output_uinteger(json, chr));
                }
                REQUIRE_OK(kefir_json_output_array_end(json));
                break;
        }
        REQUIRE_OK(kefir_json_output_object_key(json, "length"));
        REQUIRE_OK(kefir_json_output_uinteger(json, length));
        REQUIRE_OK(kefir_json_output_object_end(json));
    }
    REQUIRE(res == KEFIR_ITERATOR_END, res);
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}

static kefir_result_t format_function_declarations(struct kefir_json_output *json,
                                                   const struct kefir_ir_module *module) {
    REQUIRE_OK(kefir_json_output_array_begin(json));
    struct kefir_hashtree_node_iterator iter;
    for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(&module->function_declarations, &iter);
         node != NULL; node = kefir_hashtree_next(&iter)) {
        REQUIRE_OK(kefir_ir_format_function_declaration(json, (struct kefir_ir_function_decl *) node->value));
    }
    REQUIRE_OK(kefir_json_output_array_end(json));
    return KEFIR_OK;
}

static kefir_result_t format_functions(struct kefir_json_output *json, const struct kefir_ir_module *module) {
    REQUIRE_OK(kefir_json_output_array_begin(json));
    struct kefir_hashtree_node_iterator iter;
    for (const struct kefir_ir_function *func = kefir_ir_module_function_iter(module, &iter); func != NULL;
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
    REQUIRE_OK(kefir_json_output_object_key(json, "string_literals"));
    REQUIRE_OK(format_string_literal(json, module));
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
