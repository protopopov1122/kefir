/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kefir/parser/builder.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t free_node(struct kefir_mem *mem, struct kefir_list *list, struct kefir_list_entry *entry,
                                void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list entry"));

    ASSIGN_DECL_CAST(struct kefir_ast_node_base *, node, entry->value);
    if (node != NULL) {
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_parser_ast_builder_init(struct kefir_parser_ast_builder *builder, struct kefir_parser *parser) {
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST parser"));

    builder->parser = parser;
    REQUIRE_OK(kefir_list_init(&builder->stack));
    REQUIRE_OK(kefir_list_on_remove(&builder->stack, free_node, NULL));
    return KEFIR_OK;
}

kefir_result_t kefir_parser_ast_builder_free(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));
    REQUIRE_OK(kefir_list_free(mem, &builder->stack));
    return KEFIR_OK;
}

kefir_result_t kefir_parser_ast_builder_push(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder,
                                             struct kefir_ast_node_base *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node"));

    REQUIRE_OK(kefir_list_insert_after(mem, &builder->stack, kefir_list_tail(&builder->stack), node));
    return KEFIR_OK;
}

kefir_result_t kefir_parser_ast_builder_pop(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder,
                                            struct kefir_ast_node_base **node_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));

    struct kefir_list_entry *iter = kefir_list_tail(&builder->stack);
    REQUIRE(iter != NULL, KEFIR_SET_ERROR(KEFIR_OUT_OF_BOUNDS, "Cannot pop empty AST node stack"));
    ASSIGN_DECL_CAST(struct kefir_ast_node_base *, node, iter->value);
    iter->value = NULL;

    kefir_result_t res = kefir_list_pop(mem, &builder->stack, iter);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, node);
        return res;
    });
    if (node_ptr != NULL) {
        *node_ptr = node;
    } else {
        REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, node));
    }
    return KEFIR_OK;
}

kefir_result_t kefir_parser_ast_builder_scan(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder,
                                             kefir_parser_rule_fn_t rule, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));
    REQUIRE(rule != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST parser rule"));

    struct kefir_ast_node_base *node = NULL;
    REQUIRE_OK(kefir_parser_apply(mem, builder->parser, &node, rule, payload));
    kefir_result_t res = kefir_parser_ast_builder_push(mem, builder, node);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, node);
        return KEFIR_OK;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_parser_ast_builder_wrap(struct kefir_mem *mem, struct kefir_parser *parser,
                                             struct kefir_ast_node_base **result,
                                             kefir_parser_ast_builder_callback_t callback, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST parser"));
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to AST node"));
    REQUIRE(callback != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser AST builder callback"));

    struct kefir_parser_ast_builder builder;
    REQUIRE_OK(kefir_parser_ast_builder_init(&builder, parser));
    kefir_result_t res = callback(mem, &builder, payload);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_parser_ast_builder_free(mem, &builder);
        return res;
    });

    res = kefir_parser_ast_builder_pop(mem, &builder, result);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_parser_ast_builder_free(mem, &builder);
        return res;
    });
    REQUIRE_OK(kefir_parser_ast_builder_free(mem, &builder));
    return KEFIR_OK;
}

kefir_result_t kefir_parser_ast_builder_array_subscript(struct kefir_mem *mem,
                                                        struct kefir_parser_ast_builder *builder) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));

    struct kefir_ast_node_base *array = NULL, *subscript = NULL;
    REQUIRE_OK(kefir_parser_ast_builder_pop(mem, builder, &subscript));
    kefir_result_t res = kefir_parser_ast_builder_pop(mem, builder, &array);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, subscript);
        return KEFIR_OK;
    });

    struct kefir_ast_array_subscript *result = kefir_ast_new_array_subscript(mem, array, subscript);
    REQUIRE_ELSE(result != NULL, {
        KEFIR_AST_NODE_FREE(mem, array);
        KEFIR_AST_NODE_FREE(mem, subscript);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST array subscript");
    });

    res = kefir_parser_ast_builder_push(mem, builder, KEFIR_AST_NODE_BASE(result));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(result));
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_parser_ast_builder_function_call(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));

    struct kefir_ast_node_base *function = NULL;
    REQUIRE_OK(kefir_parser_ast_builder_pop(mem, builder, &function));

    struct kefir_ast_function_call *call = kefir_ast_new_function_call(mem, function);
    REQUIRE_ELSE(call != NULL, {
        KEFIR_AST_NODE_FREE(mem, function);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST function call");
    });

    kefir_result_t res = kefir_parser_ast_builder_push(mem, builder, KEFIR_AST_NODE_BASE(call));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(call));
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_parser_ast_builder_function_call_append(struct kefir_mem *mem,
                                                             struct kefir_parser_ast_builder *builder) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));

    struct kefir_ast_node_base *function = NULL, *arg = NULL;
    REQUIRE_OK(kefir_parser_ast_builder_pop(mem, builder, &arg));
    kefir_result_t res = kefir_parser_ast_builder_pop(mem, builder, &function);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, arg);
        return res;
    });
    REQUIRE_ELSE(function->klass->type == KEFIR_AST_FUNCTION_CALL, {
        KEFIR_AST_NODE_FREE(mem, function);
        KEFIR_AST_NODE_FREE(mem, arg);
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected function call node");
    });

    res = kefir_ast_function_call_append(mem, (struct kefir_ast_function_call *) function->self, arg);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, function);
        KEFIR_AST_NODE_FREE(mem, arg);
        return res;
    });

    res = kefir_parser_ast_builder_push(mem, builder, function);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, function);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_parser_ast_builder_struct_member(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder,
                                                      kefir_bool_t direct, const char *identifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));

    struct kefir_ast_node_base *base = NULL;
    REQUIRE_OK(kefir_parser_ast_builder_pop(mem, builder, &base));

    struct kefir_ast_struct_member *member = NULL;
    if (direct) {
        member = kefir_ast_new_struct_member(mem, builder->parser->symbols, base, identifier);
    } else {
        member = kefir_ast_new_struct_indirect_member(mem, builder->parser->symbols, base, identifier);
    }
    REQUIRE_ELSE(member != NULL, {
        KEFIR_AST_NODE_FREE(mem, base);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST struct member");
    });

    kefir_result_t res = kefir_parser_ast_builder_push(mem, builder, KEFIR_AST_NODE_BASE(member));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(member));
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_parser_ast_builder_unary_operation(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder,
                                                        kefir_ast_unary_operation_type_t operation) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));

    struct kefir_ast_node_base *base = NULL;
    REQUIRE_OK(kefir_parser_ast_builder_pop(mem, builder, &base));

    struct kefir_ast_unary_operation *oper = kefir_ast_new_unary_operation(mem, operation, base);
    REQUIRE_ELSE(oper != NULL, {
        KEFIR_AST_NODE_FREE(mem, base);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST unary operation");
    });

    kefir_result_t res = kefir_parser_ast_builder_push(mem, builder, KEFIR_AST_NODE_BASE(oper));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(oper));
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_parser_ast_builder_cast(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));

    struct kefir_ast_node_base *type_name = NULL, *expression = NULL;
    REQUIRE_OK(kefir_parser_ast_builder_pop(mem, builder, &expression));
    kefir_result_t res = kefir_parser_ast_builder_pop(mem, builder, &type_name);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, expression);
        return res;
    });
    REQUIRE_ELSE(type_name->klass->type == KEFIR_AST_TYPE_NAME, {
        KEFIR_AST_NODE_FREE(mem, type_name);
        KEFIR_AST_NODE_FREE(mem, expression);
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected type name node");
    });

    struct kefir_ast_cast_operator *cast =
        kefir_ast_new_cast_operator(mem, (struct kefir_ast_type_name *) type_name->self, expression);
    REQUIRE_ELSE(cast != NULL, {
        KEFIR_AST_NODE_FREE(mem, type_name);
        KEFIR_AST_NODE_FREE(mem, expression);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate cast operator");
    });

    res = kefir_parser_ast_builder_push(mem, builder, KEFIR_AST_NODE_BASE(cast));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(cast));
        return res;
    });
    return KEFIR_OK;
}
