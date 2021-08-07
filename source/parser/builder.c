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

kefir_result_t kefir_parser_ast_builder_binary_operation(struct kefir_mem *mem,
                                                         struct kefir_parser_ast_builder *builder,
                                                         kefir_ast_binary_operation_type_t operation) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));

    struct kefir_ast_node_base *right = NULL, *left = NULL;
    REQUIRE_OK(kefir_parser_ast_builder_pop(mem, builder, &right));
    kefir_result_t res = kefir_parser_ast_builder_pop(mem, builder, &left);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, right);
        return res;
    });

    struct kefir_ast_binary_operation *binary = kefir_ast_new_binary_operation(mem, operation, left, right);
    REQUIRE_ELSE(binary != NULL, {
        KEFIR_AST_NODE_FREE(mem, left);
        KEFIR_AST_NODE_FREE(mem, right);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate binary operation");
    });

    res = kefir_parser_ast_builder_push(mem, builder, KEFIR_AST_NODE_BASE(binary));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(binary));
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

kefir_result_t kefir_parser_ast_builder_conditional_operator(struct kefir_mem *mem,
                                                             struct kefir_parser_ast_builder *builder) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));

    struct kefir_ast_node_base *condition = NULL, *thenBranch = NULL, *elseBranch = NULL;
    REQUIRE_OK(kefir_parser_ast_builder_pop(mem, builder, &elseBranch));
    kefir_result_t res = kefir_parser_ast_builder_pop(mem, builder, &thenBranch);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, elseBranch);
        return res;
    });
    res = kefir_parser_ast_builder_pop(mem, builder, &condition);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, thenBranch);
        KEFIR_AST_NODE_FREE(mem, elseBranch);
        return res;
    });

    struct kefir_ast_conditional_operator *operator=
        kefir_ast_new_conditional_operator(mem, condition, thenBranch, elseBranch);
    REQUIRE_ELSE(operator!= NULL, {
        KEFIR_AST_NODE_FREE(mem, condition);
        KEFIR_AST_NODE_FREE(mem, thenBranch);
        KEFIR_AST_NODE_FREE(mem, elseBranch);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST conditional operator");
    });

    res = kefir_parser_ast_builder_push(mem, builder, KEFIR_AST_NODE_BASE(operator));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(operator));
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_parser_ast_builder_assignment_operator(struct kefir_mem *mem,
                                                            struct kefir_parser_ast_builder *builder,
                                                            kefir_ast_assignment_operation_t oper) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));

    struct kefir_ast_node_base *lvalue = NULL, *expression = NULL;
    REQUIRE_OK(kefir_parser_ast_builder_pop(mem, builder, &expression));
    kefir_result_t res = kefir_parser_ast_builder_pop(mem, builder, &lvalue);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, expression);
        return res;
    });

    struct kefir_ast_assignment_operator *assignment = kefir_ast_new_compound_assignment(mem, oper, lvalue, expression);
    REQUIRE_ELSE(assignment != NULL, {
        KEFIR_AST_NODE_FREE(mem, lvalue);
        KEFIR_AST_NODE_FREE(mem, expression);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate assignment operator");
    });

    res = kefir_parser_ast_builder_push(mem, builder, KEFIR_AST_NODE_BASE(assignment));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(assignment));
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_parser_ast_builder_comma_operator(struct kefir_mem *mem,
                                                       struct kefir_parser_ast_builder *builder) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));

    struct kefir_ast_node_base *expr1 = NULL, *expr2 = NULL;
    REQUIRE_OK(kefir_parser_ast_builder_pop(mem, builder, &expr2));
    kefir_result_t res = kefir_parser_ast_builder_pop(mem, builder, &expr1);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, expr2);
        return res;
    });

    if (expr1->klass->type == KEFIR_AST_COMMA_OPERATOR) {
        ASSIGN_DECL_CAST(struct kefir_ast_comma_operator *, comma, expr1->self);
        res = kefir_ast_comma_append(mem, comma, expr2);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_AST_NODE_FREE(mem, expr1);
            KEFIR_AST_NODE_FREE(mem, expr2);
            return res;
        });

        res = kefir_parser_ast_builder_push(mem, builder, expr1);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_AST_NODE_FREE(mem, expr1);
            return res;
        });
    } else {
        struct kefir_ast_comma_operator *comma = kefir_ast_new_comma_operator(mem);
        REQUIRE_ELSE(comma != NULL, {
            KEFIR_AST_NODE_FREE(mem, expr1);
            KEFIR_AST_NODE_FREE(mem, expr2);
            return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate comma operator");
        });

        res = kefir_ast_comma_append(mem, comma, expr1);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(comma));
            KEFIR_AST_NODE_FREE(mem, expr1);
            KEFIR_AST_NODE_FREE(mem, expr2);
            return res;
        });

        res = kefir_ast_comma_append(mem, comma, expr2);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(comma));
            KEFIR_AST_NODE_FREE(mem, expr2);
            return res;
        });

        res = kefir_parser_ast_builder_push(mem, builder, KEFIR_AST_NODE_BASE(comma));
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(comma));
            return res;
        });
    }
    return KEFIR_OK;
}

kefir_result_t kefir_parser_ast_builder_static_assertion(struct kefir_mem *mem,
                                                         struct kefir_parser_ast_builder *builder) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));

    struct kefir_ast_node_base *assertion = NULL, *string_literal = NULL;
    REQUIRE_OK(kefir_parser_ast_builder_pop(mem, builder, &string_literal));
    kefir_result_t res = kefir_parser_ast_builder_pop(mem, builder, &assertion);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, string_literal);
        return res;
    });

    REQUIRE_ELSE(string_literal->klass->type == KEFIR_AST_STRING_LITERAL, {
        KEFIR_AST_NODE_FREE(mem, string_literal);
        KEFIR_AST_NODE_FREE(mem, assertion);
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected string literal node");
    });

    struct kefir_ast_static_assertion *static_assertion =
        kefir_ast_new_static_assertion(mem, assertion, string_literal->self);
    REQUIRE_ELSE(static_assertion != NULL, {
        KEFIR_AST_NODE_FREE(mem, string_literal);
        KEFIR_AST_NODE_FREE(mem, assertion);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST static assertion");
    });

    res = kefir_parser_ast_builder_push(mem, builder, KEFIR_AST_NODE_BASE(static_assertion));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(static_assertion));
        return res;
    });

    return KEFIR_OK;
}

kefir_result_t kefir_parser_ast_builder_generic_selection(struct kefir_mem *mem,
                                                          struct kefir_parser_ast_builder *builder) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));

    struct kefir_ast_node_base *expr = NULL;
    REQUIRE_OK(kefir_parser_ast_builder_pop(mem, builder, &expr));

    struct kefir_ast_generic_selection *generic_selection = kefir_ast_new_generic_selection(mem, expr);
    REQUIRE_ELSE(generic_selection != NULL, {
        KEFIR_AST_NODE_FREE(mem, expr);
        return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate AST generic selection");
    });

    kefir_result_t res = kefir_parser_ast_builder_push(mem, builder, KEFIR_AST_NODE_BASE(generic_selection));
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(generic_selection));
        return res;
    });

    return KEFIR_OK;
}

kefir_result_t kefir_parser_ast_builder_generic_selection_append(struct kefir_mem *mem,
                                                                 struct kefir_parser_ast_builder *builder) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));

    struct kefir_ast_node_base *generic_selection = NULL, *type_name = NULL, *expression = NULL;
    REQUIRE_OK(kefir_parser_ast_builder_pop(mem, builder, &expression));
    kefir_result_t res = kefir_parser_ast_builder_pop(mem, builder, &type_name);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, expression);
        return res;
    });
    res = kefir_parser_ast_builder_pop(mem, builder, &generic_selection);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, type_name);
        KEFIR_AST_NODE_FREE(mem, expression);
        return res;
    });

    REQUIRE_ELSE(generic_selection->klass->type == KEFIR_AST_GENERIC_SELECTION, {
        KEFIR_AST_NODE_FREE(mem, generic_selection);
        KEFIR_AST_NODE_FREE(mem, type_name);
        KEFIR_AST_NODE_FREE(mem, expression);
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected AST generic selection node");
    });
    REQUIRE_ELSE(type_name->klass->type == KEFIR_AST_TYPE_NAME, {
        KEFIR_AST_NODE_FREE(mem, generic_selection);
        KEFIR_AST_NODE_FREE(mem, type_name);
        KEFIR_AST_NODE_FREE(mem, expression);
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected AST type name node");
    });

    res = kefir_ast_generic_selection_append(mem, generic_selection->self, type_name->self, expression);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, generic_selection);
        KEFIR_AST_NODE_FREE(mem, type_name);
        KEFIR_AST_NODE_FREE(mem, expression);
        return res;
    });

    res = kefir_parser_ast_builder_push(mem, builder, generic_selection);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, generic_selection);
        return res;
    });
    return KEFIR_OK;
}

kefir_result_t kefir_parser_ast_builder_generic_selection_append_default(struct kefir_mem *mem,
                                                                         struct kefir_parser_ast_builder *builder) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST builder"));

    struct kefir_ast_node_base *generic_selection = NULL, *expression = NULL;
    REQUIRE_OK(kefir_parser_ast_builder_pop(mem, builder, &expression));
    kefir_result_t res = kefir_parser_ast_builder_pop(mem, builder, &generic_selection);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, expression);
        return res;
    });

    REQUIRE_ELSE(generic_selection->klass->type == KEFIR_AST_GENERIC_SELECTION, {
        KEFIR_AST_NODE_FREE(mem, generic_selection);
        KEFIR_AST_NODE_FREE(mem, expression);
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected AST generic selection node");
    });

    res = kefir_ast_generic_selection_append(mem, generic_selection->self, NULL, expression);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, generic_selection);
        KEFIR_AST_NODE_FREE(mem, expression);
        return res;
    });

    res = kefir_parser_ast_builder_push(mem, builder, generic_selection);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, generic_selection);
        return res;
    });
    return KEFIR_OK;
}
