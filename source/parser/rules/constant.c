#include "kefir/parser/rule_helpers.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

DEFINE_MATCH(CONSTANT) {
    REQUIRE(PARSER_TOKEN_IS(parser, 0, KEFIR_TOKEN_CONSTANT), KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Cannot match constant"));
}
END_PROCEDURE

DEFINE_REDUCE(CONSTANT) {
    const struct kefir_token *token = PARSER_LOOKAHEAD(parser, 0);
    switch (token->constant.type) {
        case KEFIR_CONSTANT_TOKEN_INTEGER:
            ALLOC_NODE(result, kefir_ast_new_constant_int, mem, token->constant.integer);
            break;

        case KEFIR_CONSTANT_TOKEN_LONG_INTEGER:
            ALLOC_NODE(result, kefir_ast_new_constant_long, mem, token->constant.integer);
            break;

        case KEFIR_CONSTANT_TOKEN_LONG_LONG_INTEGER:
            ALLOC_NODE(result, kefir_ast_new_constant_long_long, mem, token->constant.integer);
            break;

        case KEFIR_CONSTANT_TOKEN_UNSIGNED_INTEGER:
            ALLOC_NODE(result, kefir_ast_new_constant_uint, mem, token->constant.uinteger);
            break;

        case KEFIR_CONSTANT_TOKEN_UNSIGNED_LONG_INTEGER:
            ALLOC_NODE(result, kefir_ast_new_constant_ulong, mem, token->constant.uinteger);
            break;

        case KEFIR_CONSTANT_TOKEN_UNSIGNED_LONG_LONG_INTEGER:
            ALLOC_NODE(result, kefir_ast_new_constant_ulong_long, mem, token->constant.uinteger);
            break;

        case KEFIR_CONSTANT_TOKEN_FLOAT:
            ALLOC_NODE(result, kefir_ast_new_constant_float, mem, token->constant.float32);
            break;

        case KEFIR_CONSTANT_TOKEN_DOUBLE:
            ALLOC_NODE(result, kefir_ast_new_constant_double, mem, token->constant.float64);
            break;

        case KEFIR_CONSTANT_TOKEN_CHAR:
            ALLOC_NODE(result, kefir_ast_new_constant_char, mem, token->constant.character);
            break;

        case KEFIR_CONSTANT_TOKEN_UCHAR:
            ALLOC_NODE(result, kefir_ast_new_constant_uint, mem, token->constant.unicode_char);
            break;
    }
    PARSER_SHIFT(1);
}
END_PROCEDURE

DEFINE_RULE(CONSTANT);
