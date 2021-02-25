#include "kefir/ast/node.h"
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_string_literal_visit, kefir_ast_string_literal, string_literal)

kefir_result_t ast_string_literal_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_string_literal *, node,
        base->self);
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_STRING_LITERAL_CLASS = {
    .type = KEFIR_AST_STRING_LITERAL,
    .visit = ast_string_literal_visit,
    .free = ast_string_literal_free
};

struct kefir_ast_string_literal *kefir_ast_new_string_literal(struct kefir_mem *mem,
                                                          struct kefir_symbol_table *symbols,
                                                          const char *literal) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(symbols != NULL, NULL);
    REQUIRE(literal != NULL, NULL);
    const char *literal_copy = kefir_symbol_table_insert(mem, symbols, literal, NULL);
    REQUIRE(literal_copy != NULL, NULL);
    struct kefir_ast_string_literal *string_literal = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_string_literal));
    REQUIRE(string_literal != NULL, NULL);
    string_literal->base.klass = &AST_STRING_LITERAL_CLASS;
    string_literal->base.self = string_literal;
    string_literal->base.expression_type = NULL;
    string_literal->literal = literal_copy;
    return string_literal;
}