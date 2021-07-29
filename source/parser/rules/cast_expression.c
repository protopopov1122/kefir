#include "kefir/parser/rule_helpers.h"
#include "kefir/parser/builder.h"

static kefir_result_t builder_callback(struct kefir_mem *mem, struct kefir_parser_ast_builder *builder, void *payload) {
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser AST builder"));
    struct kefir_parser *parser = builder->parser;

    kefir_result_t res = KEFIR_NO_MATCH;
    kefir_size_t checkpoint;
    REQUIRE_OK(kefir_parser_token_cursor_save(parser->cursor, &checkpoint));
    if (PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_LEFT_PARENTHESE)) {
        res = KEFIR_OK;
        REQUIRE_CHAIN(&res, PARSER_SHIFT(parser));
        REQUIRE_CHAIN(&res, kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(type_name), NULL));
        REQUIRE_CHAIN_SET(&res, PARSER_TOKEN_IS_PUNCTUATOR(parser, 0, KEFIR_PUNCTUATOR_RIGHT_PARENTHESE),
                          KEFIR_SET_ERROR(KEFIR_SYNTAX_ERROR, "Expected right parenthese"));
        REQUIRE_CHAIN(&res, PARSER_SHIFT(parser));
        REQUIRE_CHAIN(&res, kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(cast_expression), NULL));
        REQUIRE_CHAIN(&res, kefir_parser_ast_builder_cast(mem, builder));
    }
    if (res == KEFIR_NO_MATCH) {
        REQUIRE_OK(kefir_parser_token_cursor_restore(parser->cursor, checkpoint));
        REQUIRE_OK(kefir_parser_ast_builder_scan(mem, builder, KEFIR_PARSER_RULE_FN(unary_expression), NULL));
    } else {
        REQUIRE_OK(res);
    }
    return KEFIR_OK;
}

kefir_result_t KEFIR_PARSER_RULE_FN(cast_expression)(struct kefir_mem *mem, struct kefir_parser *parser,
                                                     struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    REQUIRE_OK(kefir_parser_ast_builder_wrap(mem, parser, result, builder_callback, NULL));
    return KEFIR_OK;
}
