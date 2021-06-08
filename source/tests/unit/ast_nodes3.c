#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/node.h"

DEFINE_CASE(ast_nodes_static_assertions1, "AST nodes - static assertions #1")
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_static_assertion *assert1 = kefir_ast_new_static_assertion(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 123)),
        KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Hello, goodbye!"));

    ASSERT(assert1 != NULL);
    ASSERT(assert1->condition != NULL);
    ASSERT(assert1->condition->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) assert1->condition->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) assert1->condition->self)->value.integer == 123);
    ASSERT(assert1->string != NULL);
    ASSERT(strcmp(assert1->string->literal, "Hello, goodbye!") == 0);

    struct kefir_ast_static_assertion *assert2 = kefir_ast_new_static_assertion(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(&kft_mem, KEFIR_AST_OPERATION_SUBTRACT,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)))),
        KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Something"));
    
    ASSERT(assert2 != NULL);
    ASSERT(assert2->condition != NULL);
    ASSERT(assert2->condition->klass->type == KEFIR_AST_BINARY_OPERATION);
    ASSERT(assert2->string != NULL);
    ASSERT(strcmp(assert2->string->literal, "Something") == 0);

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(assert1)));
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(assert2)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE
