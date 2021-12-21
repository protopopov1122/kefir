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

#include "kefir/ast-translator/scope/translator.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/ir/type_tree.h"
#include "kefir/ast/constant_expression.h"
#include "kefir/ast/runtime.h"
#include "kefir/ast/initializer_traversal.h"
#include "kefir/core/source_error.h"

static kefir_size_t resolve_identifier_offset(const struct kefir_ast_type_layout *layout) {
    if (layout->parent != NULL) {
        return resolve_identifier_offset(layout->parent) + layout->properties.relative_offset;
    } else {
        return layout->properties.relative_offset;
    }
}

static kefir_result_t translate_pointer_to_identifier(struct kefir_mem *mem,
                                                      struct kefir_ast_constant_expression_value *value,
                                                      struct kefir_ir_module *module, struct kefir_ir_data *data,
                                                      kefir_size_t base_slot) {
    if (value->pointer.scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT) {
        switch (value->pointer.scoped_id->object.storage) {
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN: {
                const char *literal = kefir_ir_module_symbol(mem, module, value->pointer.base.literal, NULL);
                REQUIRE(literal != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate IR symbol"));
                REQUIRE_OK(kefir_ir_data_set_pointer(data, base_slot, literal, value->pointer.offset));
            } break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC: {
                ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, identifier_data,
                                 value->pointer.scoped_id->payload.ptr);
                REQUIRE_OK(kefir_ir_data_set_pointer(
                    data, base_slot, KEFIR_AST_TRANSLATOR_STATIC_VARIABLES_IDENTIFIER,
                    resolve_identifier_offset(identifier_data->layout) + value->pointer.offset));
            } break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL:
                return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected non-constant initializer element");

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_TYPEDEF:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN:
                return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected storage class of addressed variable");
        }
    } else {
        REQUIRE(value->pointer.scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION,
                KEFIR_SET_SOURCE_ERROR(
                    KEFIR_ANALYSIS_ERROR, NULL,
                    "Global variables can be initialized by pointer either to an object or to a function"));
        const char *literal = kefir_ir_module_symbol(mem, module, value->pointer.base.literal, NULL);
        REQUIRE(literal != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate IR symbol"));
        REQUIRE_OK(kefir_ir_data_set_pointer(data, base_slot, literal, value->pointer.offset));
    }
    return KEFIR_OK;
}

struct designator_resolve_param {
    struct kefir_ir_type_tree *ir_type_tree;
    kefir_size_t *slot;
};

static kefir_result_t add_designated_slot(struct kefir_ast_type_layout *layout,
                                          const struct kefir_ast_designator *designator, void *payload) {
    REQUIRE(layout != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST type layout"));
    REQUIRE(designator != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid designator"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));

    ASSIGN_DECL_CAST(struct designator_resolve_param *, param, payload);

    const struct kefir_ir_type_tree_node *tree_node;
    REQUIRE_OK(kefir_ir_type_tree_at(param->ir_type_tree, layout->value, &tree_node));

    if (designator->type == KEFIR_AST_DESIGNATOR_MEMBER) {
        *param->slot += tree_node->relative_slot;
    } else if (designator->type == KEFIR_AST_DESIGNATOR_SUBSCRIPT) {
        *param->slot += tree_node->relative_slot + designator->index * tree_node->slot_width;
    } else {
        return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected designator type");
    }
    return KEFIR_OK;
}

static kefir_result_t resolve_designated_slot(struct kefir_ast_type_layout *root,
                                              const struct kefir_ast_designator *designator,
                                              struct kefir_ir_type_tree *ir_type_tree, kefir_size_t base_slot,
                                              struct kefir_ast_type_layout **resolved_layout, kefir_size_t *slot) {
    *resolved_layout = root;
    *slot = base_slot;
    if (designator != NULL) {
        struct designator_resolve_param param = {.ir_type_tree = ir_type_tree, .slot = slot};
        REQUIRE_OK(kefir_ast_type_layout_resolve(root, designator, resolved_layout, add_designated_slot, &param));
    }
    return KEFIR_OK;
}

struct traversal_param {
    struct kefir_mem *mem;
    const struct kefir_ast_context *context;
    struct kefir_ir_module *module;
    struct kefir_ast_type_layout *type_layout;
    const struct kefir_ir_type *type;
    struct kefir_ir_data *data;
    kefir_size_t base_slot;
    struct kefir_ir_type_tree ir_type_tree;
};

static const kefir_ir_string_literal_type_t StringLiteralTypes[] = {
    [KEFIR_AST_STRING_LITERAL_MULTIBYTE] = KEFIR_IR_STRING_LITERAL_MULTIBYTE,
    [KEFIR_AST_STRING_LITERAL_UNICODE8] = KEFIR_IR_STRING_LITERAL_MULTIBYTE,
    [KEFIR_AST_STRING_LITERAL_UNICODE16] = KEFIR_IR_STRING_LITERAL_UNICODE16,
    [KEFIR_AST_STRING_LITERAL_UNICODE32] = KEFIR_IR_STRING_LITERAL_UNICODE32,
    [KEFIR_AST_STRING_LITERAL_WIDE] = KEFIR_IR_STRING_LITERAL_UNICODE32};

static kefir_result_t visit_value(const struct kefir_ast_designator *designator, struct kefir_ast_node_base *expression,
                                  void *payload) {
    REQUIRE(expression != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST expression node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct traversal_param *, param, payload);

    struct kefir_ast_type_layout *resolved_layout = NULL;
    kefir_size_t slot = 0;
    REQUIRE_OK(resolve_designated_slot(param->type_layout, designator, &param->ir_type_tree, param->base_slot,
                                       &resolved_layout, &slot));

    struct kefir_ast_constant_expression_value uncasted_value, value;
    REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(param->mem, param->context, expression, &uncasted_value));
    REQUIRE_OK(kefir_ast_constant_expression_value_cast(param->mem, param->context, &value, &uncasted_value, expression,
                                                        resolved_layout->type));
    struct kefir_ir_typeentry *target_typeentry = kefir_ir_type_at(param->type, resolved_layout->value);
    REQUIRE(target_typeentry != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Cannot obtain target IR type entry"));
    switch (value.klass) {
        case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_NONE:
            return KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Unexpected constant expression value type");

        case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER:
            switch (target_typeentry->typecode) {
                case KEFIR_IR_TYPE_INT8:
                case KEFIR_IR_TYPE_INT16:
                case KEFIR_IR_TYPE_INT32:
                case KEFIR_IR_TYPE_INT64:
                case KEFIR_IR_TYPE_BOOL:
                case KEFIR_IR_TYPE_CHAR:
                case KEFIR_IR_TYPE_SHORT:
                case KEFIR_IR_TYPE_INT:
                case KEFIR_IR_TYPE_LONG:
                case KEFIR_IR_TYPE_WORD:
                case KEFIR_IR_TYPE_BITS:
                    if (resolved_layout->bitfield) {
                        REQUIRE_OK(kefir_ir_data_set_bitfield(param->data, slot, value.integer,
                                                              resolved_layout->bitfield_props.offset,
                                                              resolved_layout->bitfield_props.width));
                    } else {
                        REQUIRE_OK(kefir_ir_data_set_integer(param->data, slot, value.integer));
                    }
                    break;

                case KEFIR_IR_TYPE_FLOAT32:
                    REQUIRE_OK(kefir_ir_data_set_float32(param->data, slot, (kefir_float32_t) value.integer));
                    break;

                case KEFIR_IR_TYPE_FLOAT64:
                    REQUIRE_OK(kefir_ir_data_set_float64(param->data, slot, (kefir_float64_t) value.integer));
                    break;

                default:
                    return KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Unexpected target IR type entry code");
            }
            break;

        case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT:
            switch (target_typeentry->typecode) {
                case KEFIR_IR_TYPE_INT8:
                case KEFIR_IR_TYPE_INT16:
                case KEFIR_IR_TYPE_INT32:
                case KEFIR_IR_TYPE_INT64:
                case KEFIR_IR_TYPE_BOOL:
                case KEFIR_IR_TYPE_CHAR:
                case KEFIR_IR_TYPE_SHORT:
                case KEFIR_IR_TYPE_INT:
                case KEFIR_IR_TYPE_LONG:
                case KEFIR_IR_TYPE_WORD:
                    REQUIRE_OK(kefir_ir_data_set_integer(param->data, slot, (kefir_int64_t) value.floating_point));
                    break;

                case KEFIR_IR_TYPE_FLOAT32:
                    REQUIRE_OK(kefir_ir_data_set_float32(param->data, slot, value.floating_point));
                    break;

                case KEFIR_IR_TYPE_FLOAT64:
                    REQUIRE_OK(kefir_ir_data_set_float64(param->data, slot, value.floating_point));
                    break;

                default:
                    return KEFIR_SET_ERROR(KEFIR_INVALID_STATE, "Unexpected target IR type entry code");
            }
            break;

        case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS:
            switch (value.pointer.type) {
                case KEFIR_AST_CONSTANT_EXPRESSION_POINTER_IDENTIFER:
                    REQUIRE_OK(translate_pointer_to_identifier(param->mem, &value, param->module, param->data, slot));
                    break;

                case KEFIR_AST_CONSTANT_EXPRESSION_POINTER_INTEGER:
                    REQUIRE_OK(kefir_ir_data_set_integer(param->data, slot,
                                                         value.pointer.base.integral + value.pointer.offset));
                    break;

                case KEFIR_AST_CONSTANT_EXPRESSION_POINTER_LITERAL: {
                    kefir_id_t id;
                    REQUIRE_OK(kefir_ir_module_string_literal(
                        param->mem, param->module, StringLiteralTypes[value.pointer.base.string.type], true,
                        value.pointer.base.string.content, value.pointer.base.string.length, &id));
                    REQUIRE_OK(kefir_ir_data_set_string_pointer(param->data, slot, id, value.pointer.offset));
                } break;
            }
            break;
    }

    return KEFIR_OK;
}

static kefir_result_t visit_string_literal(const struct kefir_ast_designator *designator,
                                           struct kefir_ast_node_base *expression, kefir_ast_string_literal_type_t type,
                                           const void *string, kefir_size_t length, void *payload) {
    UNUSED(expression);
    REQUIRE(string != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid string literal"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct traversal_param *, param, payload);

    kefir_id_t string_id;
    REQUIRE_OK(kefir_ir_module_string_literal(param->mem, param->module, StringLiteralTypes[type], false, string,
                                              length, &string_id));

    kefir_ir_string_literal_type_t ir_string_type;
    kefir_bool_t public;
    const void *string_content = NULL;
    kefir_size_t string_length = 0;
    REQUIRE_OK(kefir_ir_module_get_string_literal(param->module, string_id, &ir_string_type, &public, &string_content,
                                                  &string_length));

    struct kefir_ast_type_layout *resolved_layout = NULL;
    kefir_size_t slot = 0;
    REQUIRE_OK(resolve_designated_slot(param->type_layout, designator, &param->ir_type_tree, param->base_slot,
                                       &resolved_layout, &slot));
    REQUIRE_OK(kefir_ir_data_set_string(param->data, slot, StringLiteralTypes[type], string_content, string_length));
    return KEFIR_OK;
}

static kefir_result_t visit_initializer_list(const struct kefir_ast_designator *designator,
                                             const struct kefir_ast_initializer *initializer, void *payload) {
    REQUIRE(initializer != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST initializer"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct traversal_param *, param, payload);

    struct kefir_ast_type_layout *resolved_layout = NULL;
    kefir_size_t slot = 0;
    REQUIRE_OK(resolve_designated_slot(param->type_layout, designator, &param->ir_type_tree, param->base_slot,
                                       &resolved_layout, &slot));

    REQUIRE_OK(kefir_ast_translate_data_initializer(param->mem, param->context, param->module, resolved_layout,
                                                    param->type, initializer, param->data, slot));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_data_initializer(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                    struct kefir_ir_module *module,
                                                    struct kefir_ast_type_layout *type_layout,
                                                    const struct kefir_ir_type *type,
                                                    const struct kefir_ast_initializer *initializer,
                                                    struct kefir_ir_data *data, kefir_size_t base_slot) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR module"));
    REQUIRE(type_layout != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST type layout"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR type"));
    REQUIRE(initializer != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST initializer"));
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid IR data"));

    struct traversal_param param = {.mem = mem,
                                    .context = context,
                                    .module = module,
                                    .type_layout = type_layout,
                                    .type = type,
                                    .data = data,
                                    .base_slot = base_slot};

    struct kefir_ast_initializer_traversal initializer_traversal;
    KEFIR_AST_INITIALIZER_TRAVERSAL_INIT(&initializer_traversal);
    initializer_traversal.visit_value = visit_value;
    initializer_traversal.visit_string_literal = visit_string_literal;
    initializer_traversal.visit_initializer_list = visit_initializer_list;
    initializer_traversal.payload = &param;

    REQUIRE_OK(kefir_ir_type_tree_init(mem, type, &param.ir_type_tree));
    kefir_result_t res =
        kefi_ast_traverse_initializer(mem, context, initializer, type_layout->type, &initializer_traversal);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_ir_type_tree_free(mem, &param.ir_type_tree);
        return res;
    });
    REQUIRE_OK(kefir_ir_type_tree_free(mem, &param.ir_type_tree));
    return KEFIR_OK;
}
