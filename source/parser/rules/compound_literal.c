#include "kefir/parser/rule_helpers.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_parser_apply_rule_compound_literal(struct kefir_mem *mem, struct kefir_parser *parser,
                                                        struct kefir_ast_node_base **result, void *payload) {
    APPLY_PROLOGUE(mem, parser, result, payload);
    // TODO Implement generic selection parser
    return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Compound literal parser is not implemented yet");
}
