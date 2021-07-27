#include "kefir/parser/rule_helpers.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_bool_t kefir_parser_match_rule_generic_selection(const struct kefir_parser *parser) {
    REQUIRE(parser != NULL, false);
    return false;
}

kefir_result_t kefir_parser_reduce_rule_generic_selection(struct kefir_mem *mem, struct kefir_parser *parser,
                                                          struct kefir_ast_node_base **result) {
    REDUCE_PROLOGUE(mem, parser, result, generic_selection);
    return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Generic selection parser is not implemented yet");
}
