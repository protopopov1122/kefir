#include "kefir/parser/parser.h"
#include "kefir/parser/rules.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_parser_init(struct kefir_mem *mem, struct kefir_parser *parser, struct kefir_symbol_table *symbols,
                                 struct kefir_parser_token_cursor *cursor) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));
    REQUIRE(cursor != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid token cursor"));

    parser->symbols = symbols;
    parser->cursor = cursor;
    return KEFIR_OK;
}

kefir_result_t kefir_parser_free(struct kefir_mem *mem, struct kefir_parser *parser) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));
    parser->cursor = NULL;
    parser->symbols = NULL;
    return KEFIR_OK;
}

kefir_result_t kefir_parser_apply(struct kefir_mem *mem, struct kefir_parser *parser,
                                  struct kefir_ast_node_base **result, kefir_parser_rule_fn_t rule, void *payload) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(parser != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser"));
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to AST node"));
    REQUIRE(rule != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid parser rule"));

    kefir_size_t checkpoint;
    REQUIRE_OK(kefir_parser_token_cursor_save(parser->cursor, &checkpoint));
    kefir_result_t res = rule(mem, parser, result, payload);
    if (res == KEFIR_NO_MATCH) {
        REQUIRE_OK(kefir_parser_token_cursor_restore(parser->cursor, checkpoint));
        return res;
    } else {
        REQUIRE_OK(res);
    }
    return KEFIR_OK;
}
