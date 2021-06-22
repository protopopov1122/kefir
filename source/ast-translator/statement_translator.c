#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/translator_impl.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

struct translator_param {
    struct kefir_mem *mem;
    struct kefir_ast_translator_context *context;
    struct kefir_irbuilder_block *builder;
};

static kefir_result_t translate_not_impl(const struct kefir_ast_visitor *visitor,
                                         const struct kefir_ast_node_base *base, void *payload) {
    UNUSED(visitor);
    UNUSED(base);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot translate non-statement AST node");
}

#define TRANSLATE_NODE(_id, _type)                                                                                     \
    static kefir_result_t translate_##_id(const struct kefir_ast_visitor *visitor, const _type *node, void *payload) { \
        REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST visitor"));                 \
        REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST node"));                       \
        REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));                      \
        ASSIGN_DECL_CAST(struct translator_param *, param, payload);                                                   \
        REQUIRE_OK(kefir_ast_translate_##_id##_node(param->mem, param->context, param->builder, node));                \
        return KEFIR_OK;                                                                                               \
    }

TRANSLATE_NODE(expression_statement, struct kefir_ast_expression_statement)
TRANSLATE_NODE(compound_statement, struct kefir_ast_compound_statement)
TRANSLATE_NODE(conditional_statement, struct kefir_ast_conditional_statement)
TRANSLATE_NODE(switch_statement, struct kefir_ast_switch_statement)
TRANSLATE_NODE(while_statement, struct kefir_ast_while_statement)
TRANSLATE_NODE(do_while_statement, struct kefir_ast_do_while_statement)
TRANSLATE_NODE(for_statement, struct kefir_ast_for_statement)
TRANSLATE_NODE(labeled_statement, struct kefir_ast_labeled_statement)
TRANSLATE_NODE(case_statement, struct kefir_ast_case_statement)
TRANSLATE_NODE(goto_statement, struct kefir_ast_goto_statement)
TRANSLATE_NODE(continue_statement, struct kefir_ast_continue_statement)
TRANSLATE_NODE(break_statement, struct kefir_ast_break_statement)
#undef TRANSLATE_NODE

kefir_result_t kefir_ast_translate_statement(struct kefir_mem *mem, const struct kefir_ast_node_base *base,
                                             struct kefir_irbuilder_block *builder,
                                             struct kefir_ast_translator_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(base != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node base"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));

    struct kefir_ast_visitor visitor;
    REQUIRE_OK(kefir_ast_visitor_init(&visitor, translate_not_impl));
    visitor.expression_statement = translate_expression_statement;
    visitor.compound_statement = translate_compound_statement;
    visitor.conditional_statement = translate_conditional_statement;
    visitor.switch_statement = translate_switch_statement;
    visitor.while_statement = translate_while_statement;
    visitor.do_while_statement = translate_do_while_statement;
    visitor.for_statement = translate_for_statement;
    visitor.labeled_statement = translate_labeled_statement;
    visitor.case_statement = translate_case_statement;
    visitor.goto_statement = translate_goto_statement;
    visitor.continue_statement = translate_continue_statement;
    visitor.break_statement = translate_break_statement;

    struct translator_param param = {.mem = mem, .builder = builder, .context = context};
    return KEFIR_AST_NODE_VISIT(&visitor, base, &param);
}
