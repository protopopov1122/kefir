#include "kefir/parser/rule_helpers.h"
#include "kefir/parser/builder.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t builder_callback(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder, void *payload) {
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser AST builder"));
    struct kefir_parser *parser = builder->parser;

    REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(unary_expression), NULL));
    if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_ASSIGN)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(assignment_expression), NULL));
        REQUIRE_OK(kefir_parser_ast_builder_assignment_operator(mem, builder, KEFIR_AST_ASSIGNMENT_SIMPLE));
    } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_ASSIGN_MULTIPLY)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(assignment_expression), NULL));
        REQUIRE_OK(kefir_parser_ast_builder_assignment_operator(mem, builder, KEFIR_AST_ASSIGNMENT_MULTIPLY));
    } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_ASSIGN_DIVIDE)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(assignment_expression), NULL));
        REQUIRE_OK(kefir_parser_ast_builder_assignment_operator(mem, builder, KEFIR_AST_ASSIGNMENT_DIVIDE));
    } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_ASSIGN_MODULO)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(assignment_expression), NULL));
        REQUIRE_OK(kefir_parser_ast_builder_assignment_operator(mem, builder, KEFIR_AST_ASSIGNMENT_MODULO));
    } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_ASSIGN_ADD)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(assignment_expression), NULL));
        REQUIRE_OK(kefir_parser_ast_builder_assignment_operator(mem, builder, KEFIR_AST_ASSIGNMENT_ADD));
    } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_ASSIGN_SUBTRACT)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(assignment_expression), NULL));
        REQUIRE_OK(kefir_parser_ast_builder_assignment_operator(mem, builder, KEFIR_AST_ASSIGNMENT_SUBTRACT));
    } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_ASSIGN_SHIFT_LEFT)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(assignment_expression), NULL));
        REQUIRE_OK(kefir_parser_ast_builder_assignment_operator(mem, builder, KEFIR_AST_ASSIGNMENT_SHIFT_LEFT));
    } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_ASSIGN_SHIFT_RIGHT)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(assignment_expression), NULL));
        REQUIRE_OK(kefir_parser_ast_builder_assignment_operator(mem, builder, KEFIR_AST_ASSIGNMENT_SHIFT_RIGHT));
    } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_ASSIGN_AND)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(assignment_expression), NULL));
        REQUIRE_OK(kefir_parser_ast_builder_assignment_operator(mem, builder, KEFIR_AST_ASSIGNMENT_BITWISE_AND));
    } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_ASSIGN_XOR)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(assignment_expression), NULL));
        REQUIRE_OK(kefir_parser_ast_builder_assignment_operator(mem, builder, KEFIR_AST_ASSIGNMENT_BITWISE_XOR));
    } else if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_ASSIGN_OR)) {
        REQUIRE_OK(PARSER_SHIFT(parser));
        REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(assignment_expression), NULL));
        REQUIRE_OK(kefir_parser_ast_builder_assignment_operator(mem, builder, KEFIR_AST_ASSIGNMENT_BITWISE_OR));
    } else {
        return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Expected assignment operator");
    }

    return KEFIR_OK;
}

static kefir_result_t reduce_assignment(struct kefir_mem *mem, struct kefir_parser *parser,
                                        struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    REQUIRE_OK(kefir_parser_ast_builder_wrap(mem, parser, result, builder_callback, NULL));
    return KEFIR_OK;
}

kefir_result_t KEFIR_PARSER_RULE_FN(assignment_expression)(struct kefir_mem *mem, struct kefir_parser *parser,
                                                           struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    kefir_result_t res = kefir_parser_apply(mem, parser, result, reduce_assignment, NULL);
    if (res == KEFIR_NO_MATCH) {
        res = KEFIR_PARSER_RULE_APPLY(mem, parser, conditional_expression, result);
    }
    REQUIRE_OK(res);
    return KEFIR_OK;
}
