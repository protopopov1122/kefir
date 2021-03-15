#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/node.h"

#define ASSERT_ASSOC(_selection, _index, _type, _expr) \
    do { \
        ASSIGN_DECL_CAST(struct kefir_ast_generic_selection_assoc *, assoc, \
            kefir_list_at(&(_selection)->associations, (_index))->value); \
        ASSERT(assoc != NULL); \
        ASSERT(KEFIR_AST_TYPE_SAME((_type), assoc->type)); \
        _expr; \
    } while (0)

DEFINE_CASE(ast_nodes_generic_selections, "AST nodes - generic selections")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    struct kefir_ast_generic_selection *selection1 = kefir_ast_new_generic_selection(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 5)));
    ASSERT(selection1 != NULL);
    ASSERT(selection1->base.klass->type == KEFIR_AST_GENERIC_SELECTION);
    ASSERT(selection1->base.self == selection1);
    ASSERT(selection1->control != NULL);
    ASSERT(selection1->control->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) selection1->control->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) selection1->control->self)->value.integer == 5);
    ASSERT(kefir_list_length(&selection1->associations) == 0);
    ASSERT(selection1->default_assoc == NULL);

    ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection1, type_traits,
        kefir_ast_type_signed_int(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true))));
    ASSERT(kefir_list_length(&selection1->associations) == 1);
    ASSERT_ASSOC(selection1, 0, kefir_ast_type_signed_int(), {
        ASSERT(assoc->expr->klass->type == KEFIR_AST_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) assoc->expr->self)->type == KEFIR_AST_BOOL_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) assoc->expr->self)->value.boolean == true);
    });
    ASSERT(selection1->default_assoc == NULL);

    struct kefir_ast_constant *cnst1 = kefir_ast_new_constant_bool(&kft_mem, true);
    ASSERT_NOK(kefir_ast_generic_selection_append(&kft_mem, selection1, type_traits,
        kefir_ast_type_signed_int(),
        KEFIR_AST_NODE_BASE(cnst1)));        
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(cnst1)));
    ASSERT(kefir_list_length(&selection1->associations) == 1);
    ASSERT(selection1->default_assoc == NULL);

    ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection1, type_traits,
        kefir_ast_type_signed_long_long(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "true"))));
    ASSERT(kefir_list_length(&selection1->associations) == 2);
    ASSERT_ASSOC(selection1, 0, kefir_ast_type_signed_int(), {
        ASSERT(assoc->expr->klass->type == KEFIR_AST_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) assoc->expr->self)->type == KEFIR_AST_BOOL_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) assoc->expr->self)->value.boolean == true);
    });
    ASSERT_ASSOC(selection1, 1, kefir_ast_type_signed_long_long(), {
        ASSERT(assoc->expr->klass->type == KEFIR_AST_IDENTIFIER);
        ASSERT(((struct kefir_ast_identifier *) assoc->expr->self)->identifier != NULL);
        ASSERT(strcmp(((struct kefir_ast_identifier *) assoc->expr->self)->identifier, "true") == 0);
    });
    ASSERT(selection1->default_assoc == NULL);

    ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection1, type_traits,
        kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_char()),
        KEFIR_AST_NODE_BASE(kefir_ast_new_string_literal(&kft_mem, &symbols, "Hello, world!"))));
    ASSERT(kefir_list_length(&selection1->associations) == 3);
    ASSERT_ASSOC(selection1, 0, kefir_ast_type_signed_int(), {
        ASSERT(assoc->expr->klass->type == KEFIR_AST_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) assoc->expr->self)->type == KEFIR_AST_BOOL_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) assoc->expr->self)->value.boolean == true);
    });
    ASSERT_ASSOC(selection1, 1, kefir_ast_type_signed_long_long(), {
        ASSERT(assoc->expr->klass->type == KEFIR_AST_IDENTIFIER);
        ASSERT(((struct kefir_ast_identifier *) assoc->expr->self)->identifier != NULL);
        ASSERT(strcmp(((struct kefir_ast_identifier *) assoc->expr->self)->identifier, "true") == 0);
    });
    ASSERT_ASSOC(selection1, 2, kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_char()), {
        ASSERT(assoc->expr->klass->type == KEFIR_AST_STRING_LITERAL);
        ASSERT(((struct kefir_ast_string_literal *) assoc->expr->self)->literal != NULL);
        ASSERT(strcmp(((struct kefir_ast_string_literal *) assoc->expr->self)->literal, "Hello, world!") == 0);
    });
    ASSERT(selection1->default_assoc == NULL);

    ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection1, type_traits,
        NULL,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'H'))));
    ASSERT(kefir_list_length(&selection1->associations) == 3);
    ASSERT_ASSOC(selection1, 0, kefir_ast_type_signed_int(), {
        ASSERT(assoc->expr->klass->type == KEFIR_AST_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) assoc->expr->self)->type == KEFIR_AST_BOOL_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) assoc->expr->self)->value.boolean == true);
    });
    ASSERT_ASSOC(selection1, 1, kefir_ast_type_signed_long_long(), {
        ASSERT(assoc->expr->klass->type == KEFIR_AST_IDENTIFIER);
        ASSERT(((struct kefir_ast_identifier *) assoc->expr->self)->identifier != NULL);
        ASSERT(strcmp(((struct kefir_ast_identifier *) assoc->expr->self)->identifier, "true") == 0);
    });
    ASSERT_ASSOC(selection1, 2, kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_char()), {
        ASSERT(assoc->expr->klass->type == KEFIR_AST_STRING_LITERAL);
        ASSERT(((struct kefir_ast_string_literal *) assoc->expr->self)->literal != NULL);
        ASSERT(strcmp(((struct kefir_ast_string_literal *) assoc->expr->self)->literal, "Hello, world!") == 0);
    });
    ASSERT(selection1->default_assoc != NULL);
    ASSERT(selection1->default_assoc->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) selection1->default_assoc->self)->type == KEFIR_AST_CHAR_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) selection1->default_assoc->self)->value.character == 'H');

    struct kefir_ast_constant *cnst2 = kefir_ast_new_constant_char(&kft_mem, 'H');
    ASSERT_NOK(kefir_ast_generic_selection_append(&kft_mem, selection1, type_traits,
        NULL,
        KEFIR_AST_NODE_BASE(cnst2)));
    ASSERT(kefir_list_length(&selection1->associations) == 3);
    ASSERT(selection1->default_assoc != NULL);
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(cnst2)));

    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(selection1)));
    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE

#undef ASSERT_ASSOC

#define ASSERT_CAST(_mem, _type, _expr, _checker) \
    do { \
        struct kefir_ast_cast_operator *cast = kefir_ast_new_cast_operator((_mem), \
            (_type), \
            (_expr)); \
        ASSERT(cast != NULL); \
        ASSERT(cast->base.klass->type == KEFIR_AST_CAST_OPERATOR); \
        ASSERT(cast->base.self == cast); \
        ASSERT(KEFIR_AST_TYPE_SAME(cast->type, (_type))); \
        _checker \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(cast))); \
    } while (0)

DEFINE_CASE(ast_nodes_cast_operators, "AST nodes - cast operators")
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    ASSERT_CAST(&kft_mem, kefir_ast_type_unsigned_char(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(&kft_mem, 100)), {
        ASSERT(cast->expr->klass->type == KEFIR_AST_CONSTANT); \
        ASSERT(((struct kefir_ast_constant *) cast->expr->self)->type == KEFIR_AST_UINT_CONSTANT); \
        ASSERT(((struct kefir_ast_constant *) cast->expr->self)->value.uinteger == 100); \
    });
    ASSERT_CAST(&kft_mem, kefir_ast_type_float(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(&kft_mem, 1234)), {
        ASSERT(cast->expr->klass->type == KEFIR_AST_CONSTANT); \
        ASSERT(((struct kefir_ast_constant *) cast->expr->self)->type == KEFIR_AST_LONG_LONG_CONSTANT); \
        ASSERT(((struct kefir_ast_constant *) cast->expr->self)->value.long_long == 1234); \
    });
    ASSERT_CAST(&kft_mem, kefir_ast_type_pointer(&kft_mem, &type_bundle, kefir_ast_type_void()),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 0)), {
        ASSERT(cast->expr->klass->type == KEFIR_AST_CONSTANT); \
        ASSERT(((struct kefir_ast_constant *) cast->expr->self)->type == KEFIR_AST_LONG_CONSTANT); \
        ASSERT(((struct kefir_ast_constant *) cast->expr->self)->value.long_integer == 0); \
    });
    ASSERT_CAST(&kft_mem, kefir_ast_type_unsigned_long(),
        KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ADDRESS, 
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "x")))), {
        ASSERT(cast->expr->klass->type == KEFIR_AST_UNARY_OPERATION); \
        ASSERT(((struct kefir_ast_unary_operation *) cast->expr->self)->type == KEFIR_AST_OPERATION_ADDRESS); \
        ASSERT(((struct kefir_ast_unary_operation *) cast->expr->self)->arg->klass->type == KEFIR_AST_IDENTIFIER); \
    });

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE

#undef ASSERT_CAST

#define ASSERT_CONDITIONAL(_mem, _condition, _expr1, _expr2, _checker) \
    do { \
        struct kefir_ast_conditional_operator *oper = kefir_ast_new_conditional_operator((_mem), \
            (_condition), (_expr1), (_expr2)); \
        ASSERT(oper != NULL); \
        ASSERT(oper->base.klass->type == KEFIR_AST_CONDITIONAL_OPERATION); \
        ASSERT(oper->base.self == oper); \
        _checker \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(oper))); \
    } while (0)

DEFINE_CASE(ast_nodes_conditional_operators, "AST nodes - conditional operators")
    struct kefir_symbol_table symbols;
    struct kefir_ast_type_bundle type_bundle;

    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

    ASSERT_CONDITIONAL(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, -1)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(&kft_mem, 2)), {
        ASSERT(oper->condition->klass->type == KEFIR_AST_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) oper->condition->self)->type == KEFIR_AST_BOOL_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) oper->condition->self)->value.boolean);

        ASSERT(oper->expr1->klass->type == KEFIR_AST_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) oper->expr1->self)->type == KEFIR_AST_INT_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) oper->expr1->self)->value.integer == -1);

        ASSERT(oper->expr2->klass->type == KEFIR_AST_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) oper->expr2->self)->type == KEFIR_AST_UINT_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) oper->expr2->self)->value.uinteger == 2);
    });

    ASSERT_CONDITIONAL(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "x")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "y")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "z")), {
        ASSERT(oper->condition->klass->type == KEFIR_AST_IDENTIFIER);
        ASSERT(strcmp(((struct kefir_ast_identifier *) oper->condition->self)->identifier, "x") == 0);

        ASSERT(oper->expr1->klass->type == KEFIR_AST_IDENTIFIER);
        ASSERT(strcmp(((struct kefir_ast_identifier *) oper->expr1->self)->identifier, "y") == 0);

        ASSERT(oper->expr2->klass->type == KEFIR_AST_IDENTIFIER);
        ASSERT(strcmp(((struct kefir_ast_identifier *) oper->expr2->self)->identifier, "z") == 0);
    });

    ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE

#undef ASSERT_CONDITIONAL