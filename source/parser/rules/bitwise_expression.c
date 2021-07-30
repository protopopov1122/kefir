#include "kefir/parser/rule_helpers.h"
#include "kefir/parser/builder.h"

#define BITWISE_RULE(_id, _expr, _punctuator, _oper)                                                                 \
    static kefir_result_t _id##_builder_callback(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder,    \
                                                 void *payload) {                                                    \
        UNUSED(payload);                                                                                             \
        REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));               \
        REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser AST builder"));         \
        struct kefir_parser *parser = builder->parser;                                                               \
                                                                                                                     \
        REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(_expr), NULL));                  \
        while (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, (_punctuator))) {                                               \
            REQUIRE_OK(PARSER_SHIFT(parser));                                                                        \
            REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(_expr), NULL));              \
            REQUIRE_OK(kefir_parser_ast_builder_binary_operation(mem, builder, (_oper)));                            \
        }                                                                                                            \
        return KEFIR_OK;                                                                                             \
    }                                                                                                                \
                                                                                                                     \
    kefir_result_t KEFIR_PARSER_RULE_FN(bitwise_##_id##_expression)(                                                 \
        struct kefir_mem * mem, struct kefir_parser * parser, struct kefir_ast_node_base * *result, void *payload) { \
        APPLY_PROLOGUE(mem, parser, result, payload);                                                                \
        REQUIRE_OK(kefir_parser_ast_builder_wrap(mem, parser, result, _id##_builder_callback, NULL));                \
        return KEFIR_OK;                                                                                             \
    }

BITWISE_RULE(and, equality_expression, KEFIR_PUNCTUATOR_AMPERSAND, KEFIR_AST_OPERATION_BITWISE_AND)
BITWISE_RULE(xor, bitwise_and_expression, KEFIR_PUNCTUATOR_CARET, KEFIR_AST_OPERATION_BITWISE_XOR)
BITWISE_RULE(or, bitwise_xor_expression, KEFIR_PUNCTUATOR_VBAR, KEFIR_AST_OPERATION_BITWISE_OR)
