#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/node.h"

#define ASSERT_ASSOC(_selection, _index, _type_name, _expr)                            \
    do {                                                                               \
        ASSIGN_DECL_CAST(struct kefir_ast_generic_selection_assoc *, assoc,            \
                         kefir_list_at(&(_selection)->associations, (_index))->value); \
        ASSERT(assoc != NULL);                                                         \
        ASSERT(assoc->type_name == (_type_name));                                      \
        _expr;                                                                         \
    } while (0)

DEFINE_CASE(ast_nodes_generic_selections, "AST nodes - generic selections")
struct kefir_symbol_table symbols;
struct kefir_ast_type_bundle type_bundle;

ASSERT_OK(kefir_symbol_table_init(&symbols));
ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

struct kefir_ast_generic_selection *selection1 =
    kefir_ast_new_generic_selection(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 5)));
ASSERT(selection1 != NULL);
ASSERT(selection1->base.klass->type == KEFIR_AST_GENERIC_SELECTION);
ASSERT(selection1->base.self == selection1);
ASSERT(selection1->control != NULL);
ASSERT(selection1->control->klass->type == KEFIR_AST_CONSTANT);
ASSERT(((struct kefir_ast_constant *) selection1->control->self)->type == KEFIR_AST_INT_CONSTANT);
ASSERT(((struct kefir_ast_constant *) selection1->control->self)->value.integer == 5);
ASSERT(kefir_list_length(&selection1->associations) == 0);
ASSERT(selection1->default_assoc == NULL);

struct kefir_ast_type_name *type_name1 =
    kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                     kefir_ast_type_specifier_int(&kft_mem)));

ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection1, type_name1,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true))));
ASSERT(kefir_list_length(&selection1->associations) == 1);
ASSERT_ASSOC(selection1, 0, type_name1, {
    ASSERT(assoc->expr->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) assoc->expr->self)->type == KEFIR_AST_BOOL_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) assoc->expr->self)->value.boolean == true);
});
ASSERT(selection1->default_assoc == NULL);

struct kefir_ast_type_name *type_name3 =
    kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name3->type_decl.specifiers,
                                                     kefir_ast_type_specifier_long(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name3->type_decl.specifiers,
                                                     kefir_ast_type_specifier_long(&kft_mem)));

ASSERT_OK(kefir_ast_generic_selection_append(
    &kft_mem, selection1, type_name3, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "true"))));
ASSERT(kefir_list_length(&selection1->associations) == 2);
ASSERT_ASSOC(selection1, 0, type_name1, {
    ASSERT(assoc->expr->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) assoc->expr->self)->type == KEFIR_AST_BOOL_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) assoc->expr->self)->value.boolean == true);
});
ASSERT_ASSOC(selection1, 1, type_name3, {
    ASSERT(assoc->expr->klass->type == KEFIR_AST_IDENTIFIER);
    ASSERT(((struct kefir_ast_identifier *) assoc->expr->self)->identifier != NULL);
    ASSERT(strcmp(((struct kefir_ast_identifier *) assoc->expr->self)->identifier, "true") == 0);
});
ASSERT(selection1->default_assoc == NULL);

struct kefir_ast_type_name *type_name4 = kefir_ast_new_type_name(
    &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name4->type_decl.specifiers,
                                                     kefir_ast_type_specifier_char(&kft_mem)));

ASSERT_OK(kefir_ast_generic_selection_append(
    &kft_mem, selection1, type_name4, KEFIR_AST_NODE_BASE(KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Hello, world!"))));
ASSERT(kefir_list_length(&selection1->associations) == 3);
ASSERT_ASSOC(selection1, 0, type_name1, {
    ASSERT(assoc->expr->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) assoc->expr->self)->type == KEFIR_AST_BOOL_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) assoc->expr->self)->value.boolean == true);
});
ASSERT_ASSOC(selection1, 1, type_name3, {
    ASSERT(assoc->expr->klass->type == KEFIR_AST_IDENTIFIER);
    ASSERT(((struct kefir_ast_identifier *) assoc->expr->self)->identifier != NULL);
    ASSERT(strcmp(((struct kefir_ast_identifier *) assoc->expr->self)->identifier, "true") == 0);
});
ASSERT_ASSOC(selection1, 2, type_name4, {
    ASSERT(assoc->expr->klass->type == KEFIR_AST_STRING_LITERAL);
    ASSERT(((struct kefir_ast_string_literal *) assoc->expr->self)->literal != NULL);
    ASSERT(strcmp(((struct kefir_ast_string_literal *) assoc->expr->self)->literal, "Hello, world!") == 0);
});
ASSERT(selection1->default_assoc == NULL);

ASSERT_OK(kefir_ast_generic_selection_append(&kft_mem, selection1, NULL,
                                             KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'H'))));
ASSERT(kefir_list_length(&selection1->associations) == 3);
ASSERT_ASSOC(selection1, 0, type_name1, {
    ASSERT(assoc->expr->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) assoc->expr->self)->type == KEFIR_AST_BOOL_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) assoc->expr->self)->value.boolean == true);
});
ASSERT_ASSOC(selection1, 1, type_name3, {
    ASSERT(assoc->expr->klass->type == KEFIR_AST_IDENTIFIER);
    ASSERT(((struct kefir_ast_identifier *) assoc->expr->self)->identifier != NULL);
    ASSERT(strcmp(((struct kefir_ast_identifier *) assoc->expr->self)->identifier, "true") == 0);
});
ASSERT_ASSOC(selection1, 2, type_name4, {
    ASSERT(assoc->expr->klass->type == KEFIR_AST_STRING_LITERAL);
    ASSERT(((struct kefir_ast_string_literal *) assoc->expr->self)->literal != NULL);
    ASSERT(strcmp(((struct kefir_ast_string_literal *) assoc->expr->self)->literal, "Hello, world!") == 0);
});
ASSERT(selection1->default_assoc != NULL);
ASSERT(selection1->default_assoc->klass->type == KEFIR_AST_CONSTANT);
ASSERT(((struct kefir_ast_constant *) selection1->default_assoc->self)->type == KEFIR_AST_CHAR_CONSTANT);
ASSERT(((struct kefir_ast_constant *) selection1->default_assoc->self)->value.character == 'H');

struct kefir_ast_constant *cnst2 = kefir_ast_new_constant_char(&kft_mem, 'H');
ASSERT_NOK(kefir_ast_generic_selection_append(&kft_mem, selection1, NULL, KEFIR_AST_NODE_BASE(cnst2)));
ASSERT(kefir_list_length(&selection1->associations) == 3);
ASSERT(selection1->default_assoc != NULL);
ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(cnst2)));

ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(selection1)));
ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE

#undef ASSERT_ASSOC

#define ASSERT_CAST(_mem, _type_name, _expr, _checker)                                                     \
    do {                                                                                                   \
        struct kefir_ast_cast_operator *cast = kefir_ast_new_cast_operator((_mem), (_type_name), (_expr)); \
        ASSERT(cast != NULL);                                                                              \
        ASSERT(cast->base.klass->type == KEFIR_AST_CAST_OPERATOR);                                         \
        ASSERT(cast->base.self == cast);                                                                   \
        ASSERT(cast->type_name == (_type_name));                                                           \
        _checker ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(cast)));                        \
    } while (0)

DEFINE_CASE(ast_nodes_cast_operators, "AST nodes - cast operators")
struct kefir_symbol_table symbols;
struct kefir_ast_type_bundle type_bundle;

ASSERT_OK(kefir_symbol_table_init(&symbols));
ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

struct kefir_ast_type_name *type_name1 =
    kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                     kefir_ast_type_specifier_unsigned(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                     kefir_ast_type_specifier_char(&kft_mem)));

ASSERT_CAST(&kft_mem, type_name1, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(&kft_mem, 100)), {
    ASSERT(cast->expr->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) cast->expr->self)->type == KEFIR_AST_UINT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) cast->expr->self)->value.uinteger == 100);
});

struct kefir_ast_type_name *type_name2 =
    kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name2->type_decl.specifiers,
                                                     kefir_ast_type_specifier_float(&kft_mem)));

ASSERT_CAST(&kft_mem, type_name2, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(&kft_mem, 1234)), {
    ASSERT(cast->expr->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) cast->expr->self)->type == KEFIR_AST_LONG_LONG_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) cast->expr->self)->value.long_long == 1234);
});

struct kefir_ast_type_name *type_name3 = kefir_ast_new_type_name(
    &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name3->type_decl.specifiers,
                                                     kefir_ast_type_specifier_void(&kft_mem)));

ASSERT_CAST(&kft_mem, type_name3, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 0)), {
    ASSERT(cast->expr->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) cast->expr->self)->type == KEFIR_AST_LONG_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) cast->expr->self)->value.long_integer == 0);
});

struct kefir_ast_type_name *type_name4 =
    kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name4->type_decl.specifiers,
                                                     kefir_ast_type_specifier_unsigned(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name4->type_decl.specifiers,
                                                     kefir_ast_type_specifier_long(&kft_mem)));

ASSERT_CAST(&kft_mem, type_name4,
            KEFIR_AST_NODE_BASE(
                kefir_ast_new_unary_operation(&kft_mem, KEFIR_AST_OPERATION_ADDRESS,
                                              KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "x")))),
            {
                ASSERT(cast->expr->klass->type == KEFIR_AST_UNARY_OPERATION);
                ASSERT(((struct kefir_ast_unary_operation *) cast->expr->self)->type == KEFIR_AST_OPERATION_ADDRESS);
                ASSERT(((struct kefir_ast_unary_operation *) cast->expr->self)->arg->klass->type ==
                       KEFIR_AST_IDENTIFIER);
            });

ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE

#undef ASSERT_CAST

#define ASSERT_CONDITIONAL(_mem, _condition, _expr1, _expr2, _checker)                    \
    do {                                                                                  \
        struct kefir_ast_conditional_operator *oper =                                     \
            kefir_ast_new_conditional_operator((_mem), (_condition), (_expr1), (_expr2)); \
        ASSERT(oper != NULL);                                                             \
        ASSERT(oper->base.klass->type == KEFIR_AST_CONDITIONAL_OPERATION);                \
        ASSERT(oper->base.self == oper);                                                  \
        _checker ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(oper)));       \
    } while (0)

DEFINE_CASE(ast_nodes_conditional_operators, "AST nodes - conditional operators")
struct kefir_symbol_table symbols;
struct kefir_ast_type_bundle type_bundle;

ASSERT_OK(kefir_symbol_table_init(&symbols));
ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

ASSERT_CONDITIONAL(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
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

ASSERT_CONDITIONAL(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "x")),
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

DEFINE_CASE(ast_nodes_assignment_operators, "AST nodes - assignment operators")
struct kefir_symbol_table symbols;
struct kefir_ast_type_bundle type_bundle;

ASSERT_OK(kefir_symbol_table_init(&symbols));
ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

kefir_ast_assignment_operation_t OPERATIONS[] = {
    KEFIR_AST_ASSIGNMENT_SIMPLE,     KEFIR_AST_ASSIGNMENT_MULTIPLY,    KEFIR_AST_ASSIGNMENT_DIVIDE,
    KEFIR_AST_ASSIGNMENT_MODULO,     KEFIR_AST_ASSIGNMENT_ADD,         KEFIR_AST_ASSIGNMENT_SUBTRACT,
    KEFIR_AST_ASSIGNMENT_SHIFT_LEFT, KEFIR_AST_ASSIGNMENT_SHIFT_RIGHT, KEFIR_AST_ASSIGNMENT_BITWISE_AND,
    KEFIR_AST_ASSIGNMENT_BITWISE_OR, KEFIR_AST_ASSIGNMENT_BITWISE_XOR};
const kefir_size_t OPERATIONS_LENGTH = sizeof(OPERATIONS) / sizeof(OPERATIONS[0]);

for (kefir_size_t i = 0; i < OPERATIONS_LENGTH; i++) {
    struct kefir_ast_assignment_operator *oper = kefir_ast_new_compound_assignment(
        &kft_mem, OPERATIONS[i], KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "x")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 100)));
    ASSERT(oper != NULL);
    ASSERT(oper->base.klass->type == KEFIR_AST_ASSIGNMENT_OPERATOR);
    ASSERT(oper->base.self == oper);
    ASSERT(oper->operation == OPERATIONS[i]);
    ASSERT(oper->target->klass->type == KEFIR_AST_IDENTIFIER);
    ASSERT(strcmp(((struct kefir_ast_identifier *) oper->target->self)->identifier, "x") == 0);
    ASSERT(oper->value->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) oper->value->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) oper->value->self)->value.integer == 100);
    ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(oper)));
}

struct kefir_ast_assignment_operator *oper =
    kefir_ast_new_simple_assignment(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "x")),
                                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 100)));
ASSERT(oper != NULL);
ASSERT(oper->base.klass->type == KEFIR_AST_ASSIGNMENT_OPERATOR);
ASSERT(oper->base.self == oper);
ASSERT(oper->operation == KEFIR_AST_ASSIGNMENT_SIMPLE);
ASSERT(oper->target->klass->type == KEFIR_AST_IDENTIFIER);
ASSERT(strcmp(((struct kefir_ast_identifier *) oper->target->self)->identifier, "x") == 0);
ASSERT(oper->value->klass->type == KEFIR_AST_CONSTANT);
ASSERT(((struct kefir_ast_constant *) oper->value->self)->type == KEFIR_AST_INT_CONSTANT);
ASSERT(((struct kefir_ast_constant *) oper->value->self)->value.integer == 100);
ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(oper)));

ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE

DEFINE_CASE(ast_nodes_comma_operators, "AST nodes - comma operators")
struct kefir_symbol_table symbols;
struct kefir_ast_type_bundle type_bundle;

ASSERT_OK(kefir_symbol_table_init(&symbols));
ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

struct kefir_ast_comma_operator *comma = kefir_ast_new_comma_operator(&kft_mem);
ASSERT(comma != NULL);
ASSERT(comma->base.klass->type == KEFIR_AST_COMMA_OPERATOR);
ASSERT(comma->base.self == comma);

for (kefir_size_t i = 0; i < 10; i++) {
    ASSERT_OK(kefir_ast_comma_append(&kft_mem, comma, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, i))));
    ASSERT(kefir_list_length(&comma->expressions) == i + 1);
    ASSIGN_DECL_CAST(struct kefir_ast_node_base *, expr, kefir_list_tail(&comma->expressions)->value);
    ASSERT(expr->klass->type == KEFIR_AST_CONSTANT);
    ASSIGN_DECL_CAST(struct kefir_ast_constant *, cnst, expr->self);
    ASSERT(cnst->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(cnst->value.integer == (kefir_int_t) i);
}

ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(comma)));

ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE

DEFINE_CASE(ast_nodes_compound_literals, "AST nodes - compound literals")
struct kefir_symbol_table symbols;
struct kefir_ast_type_bundle type_bundle;

ASSERT_OK(kefir_symbol_table_init(&symbols));
ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

struct kefir_ast_type_name *type_name1 =
    kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name1->type_decl.specifiers,
                                                     kefir_ast_type_specifier_int(&kft_mem)));

struct kefir_ast_compound_literal *compound = kefir_ast_new_compound_literal(&kft_mem, type_name1);
ASSERT(compound != NULL);
ASSERT(compound->base.klass->type == KEFIR_AST_COMPOUND_LITERAL);
ASSERT(compound->base.self == compound);
ASSERT(kefir_list_length(&compound->initializer->list.initializers) == 0);

ASSERT_OK(kefir_ast_initializer_list_append(
    &kft_mem, &compound->initializer->list,
    kefir_ast_new_member_desginator(&kft_mem, &symbols, "hello",
                                    kefir_ast_new_index_desginator(&kft_mem, 100,
                                                                   kefir_ast_new_member_desginator(&kft_mem, &symbols,
                                                                                                   "world", NULL))),
    kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)))));
ASSERT(kefir_list_length(&compound->initializer->list.initializers) == 1);

struct kefir_ast_initializer *initializer1 = kefir_ast_new_list_initializer(&kft_mem);
ASSERT_OK(kefir_ast_initializer_list_append(
    &kft_mem, &initializer1->list, NULL,
    kefir_ast_new_expression_initializer(&kft_mem,
                                         KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, "x")))));
ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &compound->initializer->list, NULL, initializer1));
ASSERT(kefir_list_length(&compound->initializer->list.initializers) == 2);
ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(compound)));

struct kefir_ast_type_name *type_name2 =
    kefir_ast_new_type_name(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
struct kefir_ast_structure_specifier *specifier1 = kefir_ast_structure_specifier_init(&kft_mem, NULL, NULL, true);
struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
                                                     kefir_ast_type_specifier_float(&kft_mem)));
ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry1,
                                                       kefir_ast_declarator_identifier(&kft_mem, &symbols, "x"), NULL));
ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));

struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
                                                     kefir_ast_type_specifier_void(&kft_mem)));
ASSERT_OK(kefir_ast_structure_declaration_entry_append(
    &kft_mem, entry2, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, &symbols, "y")),
    NULL));
ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry2));

struct kefir_ast_structure_declaration_entry *entry3 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry3->declaration.specifiers,
                                                     kefir_ast_type_specifier_bool(&kft_mem)));
ASSERT_OK(kefir_ast_structure_declaration_entry_append(&kft_mem, entry3,
                                                       kefir_ast_declarator_identifier(&kft_mem, &symbols, "z"), NULL));
ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry3));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &type_name2->type_decl.specifiers,
                                                     kefir_ast_type_specifier_struct(&kft_mem, specifier1)));

compound = kefir_ast_new_compound_literal(&kft_mem, type_name2);
ASSERT(compound != NULL);
ASSERT(compound->base.klass->type == KEFIR_AST_COMPOUND_LITERAL);
ASSERT(compound->base.self == compound);
ASSERT(kefir_list_length(&compound->initializer->list.initializers) == 0);

ASSERT_OK(kefir_ast_initializer_list_append(
    &kft_mem, &compound->initializer->list, NULL,
    kefir_ast_new_expression_initializer(&kft_mem,
                                         KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 3.14)))));
ASSERT(kefir_list_length(&compound->initializer->list.initializers) == 1);
ASSERT_OK(kefir_ast_initializer_list_append(
    &kft_mem, &compound->initializer->list, NULL,
    kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(&kft_mem, 'a')))));
ASSERT(kefir_list_length(&compound->initializer->list.initializers) == 2);
ASSERT_OK(kefir_ast_initializer_list_append(
    &kft_mem, &compound->initializer->list, NULL,
    kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)))));
ASSERT(kefir_list_length(&compound->initializer->list.initializers) == 3);
ASSERT_OK(kefir_ast_initializer_list_append(
    &kft_mem, &compound->initializer->list, NULL,
    kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 5)))));
ASSERT(kefir_list_length(&compound->initializer->list.initializers) == 4);
ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(compound)));

ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE

DEFINE_CASE(ast_nodes_declarations1, "AST nodes - declarations #1")
struct kefir_symbol_table symbols;
struct kefir_ast_type_bundle type_bundle;

ASSERT_OK(kefir_symbol_table_init(&symbols));
ASSERT_OK(kefir_ast_type_bundle_init(&type_bundle, &symbols));

struct kefir_ast_declaration *decl1 =
    kefir_ast_new_declaration(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL), NULL);
ASSERT(decl1 != NULL);
ASSERT(decl1->declarator->klass == KEFIR_AST_DECLARATOR_IDENTIFIER);
ASSERT(decl1->declarator->identifier == NULL);
ASSERT(kefir_ast_declarator_specifier_list_iter(&decl1->specifiers, NULL) == NULL);

ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_specifier_unsigned(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_specifier_long(&kft_mem)));
ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &decl1->specifiers,
                                                     kefir_ast_type_qualifier_const(&kft_mem)));

struct kefir_ast_declarator_specifier *specifier = NULL;
struct kefir_list_entry *iter = kefir_ast_declarator_specifier_list_iter(&decl1->specifiers, &specifier);
ASSERT(iter != NULL);
ASSERT(specifier->klass == KEFIR_AST_TYPE_SPECIFIER);
ASSERT(specifier->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_UNSIGNED);
ASSERT_OK(kefir_ast_declarator_specifier_list_next(&iter, &specifier));
ASSERT(iter != NULL);
ASSERT(specifier->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_LONG);
ASSERT_OK(kefir_ast_declarator_specifier_list_next(&iter, &specifier));
ASSERT(iter != NULL);
ASSERT(specifier->type_qualifier == KEFIR_AST_TYPE_QUALIFIER_CONST);
ASSERT_OK(kefir_ast_declarator_specifier_list_next(&iter, &specifier));
ASSERT(iter == NULL);
ASSERT(decl1->initializer == NULL);

struct kefir_ast_initializer *initializer2 =
    kefir_ast_new_expression_initializer(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)));

struct kefir_ast_declaration *decl2 = kefir_ast_new_declaration(
    &kft_mem,
    kefir_ast_declarator_pointer(
        &kft_mem, kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL))),
    initializer2);
ASSERT(decl2 != NULL);
ASSERT(decl2->declarator->klass == KEFIR_AST_DECLARATOR_POINTER);
ASSERT(kefir_ast_type_qualifier_list_iter(&decl2->declarator->pointer.type_qualifiers, NULL) == NULL);
ASSERT(decl2->declarator->pointer.declarator->klass == KEFIR_AST_DECLARATOR_POINTER);
ASSERT(kefir_ast_type_qualifier_list_iter(&decl2->declarator->pointer.declarator->pointer.type_qualifiers, NULL) ==
       NULL);
ASSERT(decl2->declarator->pointer.declarator->pointer.declarator->klass == KEFIR_AST_DECLARATOR_IDENTIFIER);
ASSERT(decl2->declarator->pointer.declarator->pointer.declarator->identifier == NULL);
ASSERT(kefir_ast_declarator_specifier_list_iter(&decl2->specifiers, NULL) == NULL);
ASSERT(decl2->initializer == initializer2);

ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl1)));
ASSERT_OK(KEFIR_AST_NODE_FREE(&kft_mem, KEFIR_AST_NODE_BASE(decl2)));
ASSERT_OK(kefir_ast_type_bundle_free(&kft_mem, &type_bundle));
ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE
