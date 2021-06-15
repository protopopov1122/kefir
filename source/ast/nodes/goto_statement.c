#include "kefir/ast/node.h"
#include "kefir/ast/node_internal.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

NODE_VISIT_IMPL(ast_goto_statement_visit, kefir_ast_goto_statement, goto_statement)

struct kefir_ast_node_base *ast_goto_statement_clone(struct kefir_mem *, struct kefir_ast_node_base *);

kefir_result_t ast_goto_statement_free(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    ASSIGN_DECL_CAST(struct kefir_ast_goto_statement *, node, base->self);
    node->identifier = NULL;
    KEFIR_FREE(mem, node);
    return KEFIR_OK;
}

const struct kefir_ast_node_class AST_GOTO_STATEMENT_CLASS = {.type = KEFIR_AST_GOTO_STATEMENT,
                                                              .visit = ast_goto_statement_visit,
                                                              .clone = ast_goto_statement_clone,
                                                              .free = ast_goto_statement_free};

struct kefir_ast_node_base *ast_goto_statement_clone(struct kefir_mem *mem, struct kefir_ast_node_base *base) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(base != NULL, NULL);
    ASSIGN_DECL_CAST(struct kefir_ast_goto_statement *, node, base->self);
    struct kefir_ast_goto_statement *clone = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_goto_statement));
    REQUIRE(clone != NULL, NULL);
    clone->base.klass = &AST_GOTO_STATEMENT_CLASS;
    clone->base.self = clone;
    kefir_result_t res = kefir_ast_node_properties_clone(&clone->base.properties, &node->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, clone);
        return NULL;
    });

    clone->identifier = node->identifier;
    return KEFIR_AST_NODE_BASE(clone);
}

struct kefir_ast_goto_statement *kefir_ast_new_goto_statement(struct kefir_mem *mem, struct kefir_symbol_table *symbols,
                                                              const char *identifier) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(identifier != NULL, NULL);

    if (symbols != NULL) {
        identifier = kefir_symbol_table_insert(mem, symbols, identifier, NULL);
        REQUIRE(identifier != NULL, NULL);
    }

    struct kefir_ast_goto_statement *id = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_goto_statement));
    REQUIRE(id != NULL, NULL);
    id->base.klass = &AST_GOTO_STATEMENT_CLASS;
    id->base.self = id;
    kefir_result_t res = kefir_ast_node_properties_init(&id->base.properties);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, id);
        return NULL;
    });

    id->identifier = identifier;
    return id;
}
