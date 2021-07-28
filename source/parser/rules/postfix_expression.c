#include "kefir/parser/rule_helpers.h"

static kefir_result_t scan_subscript(struct kefir_mem *mem, struct kefir_parser *parser,
                                     struct kefir_ast_node_base **index) {
    REQUIRE_OK(PARSER_SHIFT(parser));
    REQUIRE_OK(KEFIR_PARSER_RULE_APPLY(mem, parser, expression, index));
    REQUIRE_ELSE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_BRACKET), {
        KEFIR_AST_NODE_FREE(mem, *index);
        return KEFIR_SET_ERROR(KEFIR_SYNTAX_ERROR, "Expected right bracket");
    });
    kefir_result_t res = PARSER_SHIFT(parser);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_AST_NODE_FREE(mem, *index);
        return res;
    });
    return KEFIR_OK;
}

static kefir_result_t scan_argument_list(struct kefir_mem *mem, struct kefir_parser *parser,
                                         struct kefir_ast_function_call *call) {
    REQUIRE_OK(PARSER_SHIFT(parser));
    while (!PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE)) {
        struct kefir_ast_node_base *arg = NULL;
        REQUIRE_OK(KEFIR_PARSER_RULE_APPLY(mem, parser, assignment_expression, &arg));
        kefir_result_t res = kefir_ast_function_call_append(mem, call, arg);
        REQUIRE_ELSE(res == KEFIR_OK, {
            KEFIR_AST_NODE_FREE(mem, arg);
            return res;
        });

        if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_COMMA)) {
            REQUIRE_OK(PARSER_SHIFT(parser));
        } else {
            REQUIRE(PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE),
                    KEFIR_SET_ERROR(KEFIR_SYNTAX_ERROR, "Expected either comma, or right parenthese"));
        }
    }
    REQUIRE_OK(PARSER_SHIFT(parser));
    return KEFIR_OK;
}

static kefir_result_t scan_member(struct kefir_mem *mem, struct kefir_parser *parser,
                                  struct kefir_ast_node_base **base) {
    kefir_bool_t direct = PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_DOT);
    REQUIRE_OK(PARSER_SHIFT(parser));
    REQUIRE(PARSER_TOKEN_IS_IDENTIFIER(parser, 0), KEFIR_SET_ERROR(KEFIR_SYNTAX_ERROR, "Expected identifier"));
    const struct kefir_token *token = PARSER_CURSOR(parser, 0);
    REQUIRE_OK(PARSER_SHIFT(parser));
    struct kefir_ast_struct_member *member = NULL;
    if (direct) {
        member = kefir_ast_new_struct_member(mem, parser->symbols, *base, token->identifier);
    } else {
        member = kefir_ast_new_struct_indirect_member(mem, parser->symbols, *base, token->identifier);
    }
    REQUIRE(member != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate struct member node"));
    *base = KEFIR_AST_NODE_BASE(member);
    return KEFIR_OK;
}

static kefir_result_t scan_postfixes(struct kefir_mem *mem, struct kefir_parser *parser,
                                     struct kefir_ast_node_base **result) {
    struct kefir_ast_node_base *node = NULL;
    kefir_bool_t scan_postfix = true;

    REQUIRE_OK(KEFIR_PARSER_RULE_APPLY(mem, parser, primary_expression, &node));
    do {
        kefir_result_t res = KEFIR_OK;
        if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_BRACKET)) {
            struct kefir_ast_node_base *index = NULL;
            res = scan_subscript(mem, parser, &index);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_AST_NODE_FREE(mem, node);
                return res;
            });
            struct kefir_ast_array_subscript *new_node = kefir_ast_new_array_subscript(mem, node, index);
            REQUIRE_ELSE(new_node != NULL, {
                KEFIR_AST_NODE_FREE(mem, node);
                return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate array subscript node");
            });
            node = KEFIR_AST_NODE_BASE(new_node);
        } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE)) {
            struct kefir_ast_function_call *new_node = kefir_ast_new_function_call(mem, node);
            REQUIRE_ELSE(new_node != NULL, {
                KEFIR_AST_NODE_FREE(mem, node);
                return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate function call node");
            });
            res = scan_argument_list(mem, parser, new_node);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_AST_NODE_FREE(mem, KEFIR_AST_NODE_BASE(new_node));
                return res;
            });
            node = KEFIR_AST_NODE_BASE(new_node);
        } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_DOT) ||
                   PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_ARROW)) {
            res = scan_member(mem, parser, &node);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_AST_NODE_FREE(mem, node);
                return res;
            });
        } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_PLUS_PLUS)) {
            res = PARSER_SHIFT(parser);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_AST_NODE_FREE(mem, node);
                return res;
            });

            struct kefir_ast_unary_operation *new_node =
                kefir_ast_new_unary_operation(mem, KEFIR_AST_OPERATION_POSTFIX_INCREMENT, node);
            REQUIRE_ELSE(new_node != NULL, {
                KEFIR_AST_NODE_FREE(mem, node);
                return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate unary operation node");
            });
            node = KEFIR_AST_NODE_BASE(new_node);
        } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_MINUS_MINUS)) {
            res = PARSER_SHIFT(parser);
            REQUIRE_ELSE(res == KEFIR_OK, {
                KEFIR_AST_NODE_FREE(mem, node);
                return res;
            });

            struct kefir_ast_unary_operation *new_node =
                kefir_ast_new_unary_operation(mem, KEFIR_AST_OPERATION_POSTFIX_DECREMENT, node);
            REQUIRE_ELSE(new_node != NULL, {
                KEFIR_AST_NODE_FREE(mem, node);
                return KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate unary operation node");
            });
            node = KEFIR_AST_NODE_BASE(new_node);
        } else {
            scan_postfix = false;
        }
    } while (scan_postfix);

    *result = node;
    return KEFIR_OK;
}

kefir_result_t kefir_parser_apply_rule_postfix_expression(struct kefir_mem *mem, struct kefir_parser *parser,
                                                          struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);

    kefir_result_t res = KEFIR_PARSER_RULE_APPLY(mem, parser, compound_literal, result);
    if (res == KEFIR_NO_MATCH) {
        REQUIRE_OK(scan_postfixes(mem, parser, result));
    } else {
        REQUIRE_OK(res);
    }
    return KEFIR_OK;
}
