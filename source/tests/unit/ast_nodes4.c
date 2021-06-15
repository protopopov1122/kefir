#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/node.h"

DEFINE_CASE(ast_nodes_goto_statements1, "AST nodes - goto statements #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_goto_statement *goto1 = kefir_ast_new_goto_statement(&kft_mem, &symbols, "label1");
    ASSERT(goto1 != NULL);
    ASSERT(goto1->base.klass->type == KEFIR_AST_GOTO_STATEMENT);
    ASSERT(goto1->base.self == goto1);
    ASSERT(goto1->identifier != NULL);
    ASSERT(strcmp(goto1->identifier, "label1") == 0);

    struct kefir_ast_goto_statement *goto2 = kefir_ast_new_goto_statement(&kft_mem, &symbols, "label2");
    ASSERT(goto2 != NULL);
    ASSERT(goto2->base.klass->type == KEFIR_AST_GOTO_STATEMENT);
    ASSERT(goto2->base.self == goto2);
    ASSERT(goto2->identifier != NULL);
    ASSERT(strcmp(goto2->identifier, "label2") == 0);

    struct kefir_ast_goto_statement *goto3 = kefir_ast_new_goto_statement(&kft_mem, NULL, "label3");
    ASSERT(goto3 != NULL);
    ASSERT(goto3->base.klass->type == KEFIR_AST_GOTO_STATEMENT);
    ASSERT(goto3->base.self == goto3);
    ASSERT(goto3->identifier != NULL);
    ASSERT(strcmp(goto3->identifier, "label3") == 0);

    struct kefir_ast_goto_statement *goto4 = kefir_ast_new_goto_statement(&kft_mem, NULL, NULL);
    ASSERT(goto4 == NULL);
    struct kefir_ast_goto_statement *goto5 = kefir_ast_new_goto_statement(&kft_mem, &symbols, NULL);
    ASSERT(goto5 == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(goto1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(goto2)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(goto3)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_continue_statements, "AST nodes - continue statements") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_continue_statement *continue1 = kefir_ast_new_continue_statement(&kft_mem);
    ASSERT(continue1 != NULL);
    ASSERT(continue1->base.klass->type == KEFIR_AST_CONTINUE_STATEMENT);
    ASSERT(continue1->base.self == continue1);

    struct kefir_ast_continue_statement *continue2 = kefir_ast_new_continue_statement(&kft_mem);
    ASSERT(continue2 != NULL);
    ASSERT(continue2->base.klass->type == KEFIR_AST_CONTINUE_STATEMENT);
    ASSERT(continue2->base.self == continue2);
    ASSERT(continue1 != continue2);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(continue1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(continue2)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_break_statements, "AST nodes - break statements") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_break_statement *break1 = kefir_ast_new_break_statement(&kft_mem);
    ASSERT(break1 != NULL);
    ASSERT(break1->base.klass->type == KEFIR_AST_BREAK_STATEMENT);
    ASSERT(break1->base.self == break1);

    struct kefir_ast_break_statement *break2 = kefir_ast_new_break_statement(&kft_mem);
    ASSERT(break2 != NULL);
    ASSERT(break2->base.klass->type == KEFIR_AST_BREAK_STATEMENT);
    ASSERT(break2->base.self == break2);
    ASSERT(break1 != break2);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(break1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(break2)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_nodes_return_statements1, "AST nodes - return statements #1") {
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_return_statement *return1 =
        kefir_ast_new_return_statement(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)));
    ASSERT(return1 != NULL);
    ASSERT(return1->base.klass->type == KEFIR_AST_RETURN_STATEMENT);
    ASSERT(return1->base.self == return1);
    ASSERT(return1->expression != NULL);
    ASSERT(return1->expression->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) return1->expression->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) return1->expression->self)->value.integer == 1);

    struct kefir_ast_return_statement *return2 = kefir_ast_new_return_statement(&kft_mem, NULL);
    ASSERT(return2 != NULL);
    ASSERT(return2->base.klass->type == KEFIR_AST_RETURN_STATEMENT);
    ASSERT(return2->base.self == return2);
    ASSERT(return2->expression == NULL);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(return1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(return2)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE
