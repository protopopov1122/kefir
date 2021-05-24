#include "kefir/ast/declarator.h"
#include "kefir/ast/node.h"
#include "kefir/test/unit_test.h"
#include <string.h>

DEFINE_CASE(ast_declarator_construction1, "AST declarators - identifier construction #1")
    ASSERT(kefir_ast_declarator_identifier(NULL, NULL, NULL) == NULL);

    struct kefir_ast_declarator *decl = kefir_ast_declarator_identifier(&kft_mem, NULL, NULL);
    ASSERT(decl != NULL);
    ASSERT(decl->klass == KEFIR_AST_DECLARATOR_IDENTIFIER);
    ASSERT(decl->identifier == NULL);

    kefir_bool_t abstract;
    ASSERT_OK(kefir_ast_declarator_is_abstract(decl, &abstract));
    ASSERT(abstract);
    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, decl));
END_CASE

DEFINE_CASE(ast_declarator_construction2, "AST declarators - identifier construction #2")
    const char *IDENTS[] = {
        "a",
        "abc",
        "test_123_test_56",
        "quite_long_identifier_here",
        "reallyReallyLongIdentifierHere"
    };
    const kefir_size_t IDENT_COUNT = sizeof(IDENTS) / sizeof(IDENTS[0]);

    for (kefir_size_t i = 0; i < IDENT_COUNT; i++) {
        ASSERT(kefir_ast_declarator_identifier(NULL, NULL, IDENTS[i]) == NULL);
        struct kefir_ast_declarator *decl = kefir_ast_declarator_identifier(&kft_mem, NULL, IDENTS[i]);
        ASSERT(decl != NULL);
        ASSERT(decl->klass == KEFIR_AST_DECLARATOR_IDENTIFIER);
        ASSERT(decl->identifier == IDENTS[i]);
        ASSERT(strcmp(decl->identifier, IDENTS[i]) == 0);

        kefir_bool_t abstract;
        ASSERT_OK(kefir_ast_declarator_is_abstract(decl, &abstract));
        ASSERT(!abstract);
        ASSERT_OK(kefir_ast_declarator_free(&kft_mem, decl));
    }
END_CASE

DEFINE_CASE(ast_declarator_construction3, "AST declarators - identifier construction #3")
    const char *IDENTS[] = {
        "a",
        "abc",
        "test_123_test_56",
        "quite_long_identifier_here",
        "reallyReallyLongIdentifierHere"
    };
    const kefir_size_t IDENT_COUNT = sizeof(IDENTS) / sizeof(IDENTS[0]);
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));
    ASSERT(kefir_ast_declarator_identifier(NULL, &symbols, NULL) == NULL);

    for (kefir_size_t i = 0; i < IDENT_COUNT; i++) {
        ASSERT(kefir_ast_declarator_identifier(NULL, &symbols, IDENTS[i]) == NULL);
        struct kefir_ast_declarator *decl = kefir_ast_declarator_identifier(&kft_mem, &symbols, IDENTS[i]);
        ASSERT(decl != NULL);
        ASSERT(decl->klass == KEFIR_AST_DECLARATOR_IDENTIFIER);
        ASSERT(decl->identifier != IDENTS[i]);
        ASSERT(strcmp(decl->identifier, IDENTS[i]) == 0);

        kefir_bool_t abstract;
        ASSERT_OK(kefir_ast_declarator_is_abstract(decl, &abstract));
        ASSERT(!abstract);
        ASSERT_OK(kefir_ast_declarator_free(&kft_mem, decl));
    }

    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE

DEFINE_CASE(ast_declarator_construction4, "AST declarators - pointer construction #1")
    struct kefir_ast_declarator *decl = kefir_ast_declarator_pointer(&kft_mem,
        kefir_ast_declarator_identifier(&kft_mem, NULL, "test123"));
    ASSERT(decl != NULL);
    ASSERT(decl->klass = KEFIR_AST_DECLARATOR_POINTER);
    ASSERT(decl->pointer.declarator != NULL);
    ASSERT(decl->pointer.declarator->klass == KEFIR_AST_DECLARATOR_IDENTIFIER);
    ASSERT(strcmp(decl->pointer.declarator->identifier, "test123") == 0);
    ASSERT(kefir_ast_type_qualifier_list_iter(&decl->pointer.type_qualifiers, NULL) == NULL);

    kefir_bool_t abstract;
    ASSERT_OK(kefir_ast_declarator_is_abstract(decl, &abstract));
    ASSERT(!abstract);

    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &decl->pointer.type_qualifiers, KEFIR_AST_TYPE_QUALIFIER_CONST));
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &decl->pointer.type_qualifiers, KEFIR_AST_TYPE_QUALIFIER_VOLATILE));
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &decl->pointer.type_qualifiers, KEFIR_AST_TYPE_QUALIFIER_RESTRICT));
    ASSERT_OK(kefir_ast_type_qualifier_list_remove(&kft_mem, &decl->pointer.type_qualifiers,
        kefir_ast_type_qualifier_list_iter(&decl->pointer.type_qualifiers, NULL)));

    kefir_ast_type_qualifier_type_t type_qualifier;
    const struct kefir_list_entry *iter = kefir_ast_type_qualifier_list_iter(&decl->pointer.type_qualifiers, &type_qualifier);
    ASSERT(iter != NULL);
    ASSERT(type_qualifier == KEFIR_AST_TYPE_QUALIFIER_VOLATILE);
    ASSERT_OK(kefir_ast_type_qualifier_list_next(&iter, &type_qualifier));
    ASSERT(iter != NULL);
    ASSERT(type_qualifier == KEFIR_AST_TYPE_QUALIFIER_RESTRICT);
    ASSERT_OK(kefir_ast_type_qualifier_list_next(&iter, &type_qualifier));
    ASSERT(iter == NULL);

    ASSERT_OK(kefir_ast_declarator_is_abstract(decl, &abstract));
    ASSERT(!abstract);

    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, decl));
END_CASE

DEFINE_CASE(ast_declarator_construction5, "AST declarators - pointer construction #2")
    struct kefir_ast_declarator *decl = kefir_ast_declarator_pointer(&kft_mem,
        kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, "some_test")));
    ASSERT(decl != NULL);
    ASSERT(decl->klass = KEFIR_AST_DECLARATOR_POINTER);
    ASSERT(decl->pointer.declarator != NULL);
    ASSERT(decl->pointer.declarator->klass == KEFIR_AST_DECLARATOR_POINTER);
    ASSERT(decl->pointer.declarator->pointer.declarator != NULL);
    ASSERT(decl->pointer.declarator->pointer.declarator->klass == KEFIR_AST_DECLARATOR_IDENTIFIER);
    ASSERT(strcmp(decl->pointer.declarator->pointer.declarator->identifier, "some_test") == 0);
    ASSERT(kefir_ast_type_qualifier_list_iter(&decl->pointer.type_qualifiers, NULL) == NULL);
    ASSERT(kefir_ast_type_qualifier_list_iter(&decl->pointer.declarator->pointer.type_qualifiers, NULL) == NULL);

    kefir_bool_t abstract;
    ASSERT_OK(kefir_ast_declarator_is_abstract(decl, &abstract));
    ASSERT(!abstract);
    ASSERT_OK(kefir_ast_declarator_is_abstract(decl->pointer.declarator, &abstract));
    ASSERT(!abstract);

    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &decl->pointer.type_qualifiers, KEFIR_AST_TYPE_QUALIFIER_CONST));
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &decl->pointer.declarator->pointer.type_qualifiers,
        KEFIR_AST_TYPE_QUALIFIER_VOLATILE));

    kefir_ast_type_qualifier_type_t type_qualifier;
    const struct kefir_list_entry *iter = kefir_ast_type_qualifier_list_iter(&decl->pointer.type_qualifiers, &type_qualifier);
    ASSERT(iter != NULL);
    ASSERT(type_qualifier == KEFIR_AST_TYPE_QUALIFIER_CONST);
    ASSERT_OK(kefir_ast_type_qualifier_list_next(&iter, &type_qualifier));
    ASSERT(iter == NULL);

    iter = kefir_ast_type_qualifier_list_iter(&decl->pointer.declarator->pointer.type_qualifiers, &type_qualifier);
    ASSERT(iter != NULL);
    ASSERT(type_qualifier == KEFIR_AST_TYPE_QUALIFIER_VOLATILE);
    ASSERT_OK(kefir_ast_type_qualifier_list_next(&iter, &type_qualifier));
    ASSERT(iter == NULL);

    ASSERT_OK(kefir_ast_declarator_is_abstract(decl, &abstract));
    ASSERT(!abstract);
    ASSERT_OK(kefir_ast_declarator_is_abstract(decl->pointer.declarator, &abstract));
    ASSERT(!abstract);

    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, decl));
END_CASE

DEFINE_CASE(ast_declarator_construction6, "AST declarators - pointer construction #3")
    ASSERT(kefir_ast_declarator_pointer(&kft_mem, NULL) == NULL);
    ASSERT(kefir_ast_declarator_pointer(NULL, NULL) == NULL);

    struct kefir_ast_declarator *decl1 = kefir_ast_declarator_pointer(&kft_mem,
        kefir_ast_declarator_identifier(&kft_mem, NULL, NULL));
    struct kefir_ast_declarator *decl2 = kefir_ast_declarator_pointer(&kft_mem,
        kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));

    kefir_bool_t abstract;
    ASSERT_OK(kefir_ast_declarator_is_abstract(decl1, &abstract));
    ASSERT(abstract);
    ASSERT_OK(kefir_ast_declarator_is_abstract(decl2, &abstract));
    ASSERT(abstract);

    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, decl1));
    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, decl2));
END_CASE

DEFINE_CASE(ast_declarator_construction7, "AST declarators - array construction #1")
    struct kefir_ast_declarator *decl = kefir_ast_declarator_array(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 100)),
        kefir_ast_declarator_identifier(&kft_mem, NULL, "array"));

    ASSERT(decl != NULL);
    ASSERT(decl->klass == KEFIR_AST_DECLARATOR_ARRAY);
    ASSERT(decl->array.declarator != NULL);
    ASSERT(decl->array.declarator->klass == KEFIR_AST_DECLARATOR_IDENTIFIER);
    ASSERT(strcmp(decl->array.declarator->identifier, "array") == 0);
    ASSERT(decl->array.length != NULL);
    ASSERT(decl->array.length->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) decl->array.length->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) decl->array.length->self)->value.integer == 100);
    ASSERT(!decl->array.static_array);
    ASSERT(kefir_ast_type_qualifier_list_iter(&decl->array.type_qualifiers, NULL) == NULL);

    kefir_bool_t abstract;
    ASSERT_OK(kefir_ast_declarator_is_abstract(decl, &abstract));
    ASSERT(!abstract);

    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &decl->array.type_qualifiers, KEFIR_AST_TYPE_QUALIFIER_VOLATILE));
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &decl->array.type_qualifiers, KEFIR_AST_TYPE_QUALIFIER_ATOMIC));
    ASSERT_OK(kefir_ast_type_qualifier_list_append(&kft_mem, &decl->array.type_qualifiers, KEFIR_AST_TYPE_QUALIFIER_CONST));
    ASSERT_OK(kefir_ast_type_qualifier_list_remove(&kft_mem, &decl->array.type_qualifiers,
        kefir_ast_type_qualifier_list_iter(&decl->array.type_qualifiers, NULL)));

    kefir_ast_type_qualifier_type_t type_qualifier;
    const struct kefir_list_entry *iter = kefir_ast_type_qualifier_list_iter(&decl->array.type_qualifiers, &type_qualifier);
    ASSERT(iter != NULL);
    ASSERT(type_qualifier == KEFIR_AST_TYPE_QUALIFIER_ATOMIC);
    ASSERT_OK(kefir_ast_type_qualifier_list_next(&iter, &type_qualifier));
    ASSERT(iter != NULL);
    ASSERT(type_qualifier == KEFIR_AST_TYPE_QUALIFIER_CONST);
    ASSERT_OK(kefir_ast_type_qualifier_list_next(&iter, &type_qualifier));
    ASSERT(iter == NULL);

    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, decl));
END_CASE

DEFINE_CASE(ast_declarator_construction8, "AST declarators - array construction #2")
    struct kefir_ast_declarator *decl = kefir_ast_declarator_array(&kft_mem,
        NULL,
        kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, "array2")));

    ASSERT(decl != NULL);
    ASSERT(decl->klass == KEFIR_AST_DECLARATOR_ARRAY);
    ASSERT(decl->array.declarator != NULL);
    ASSERT(decl->array.declarator->klass == KEFIR_AST_DECLARATOR_POINTER);
    ASSERT(decl->array.length == NULL);
    ASSERT(!decl->array.static_array);
    ASSERT(kefir_ast_type_qualifier_list_iter(&decl->array.type_qualifiers, NULL) == NULL);

    kefir_bool_t abstract;
    ASSERT_OK(kefir_ast_declarator_is_abstract(decl, &abstract));
    ASSERT(!abstract);

    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, decl));
END_CASE

DEFINE_CASE(ast_declarator_construction9, "AST declarators - array construction #3")
    ASSERT(kefir_ast_declarator_array(&kft_mem, NULL, NULL) == NULL);
    ASSERT(kefir_ast_declarator_array(NULL, NULL, NULL) == NULL);

    struct kefir_ast_declarator *decl = kefir_ast_declarator_array(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
        kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));

    ASSERT(decl != NULL);
    ASSERT(decl->klass == KEFIR_AST_DECLARATOR_ARRAY);
    ASSERT(decl->array.declarator != NULL);
    ASSERT(decl->array.declarator->klass == KEFIR_AST_DECLARATOR_POINTER);
    ASSERT(decl->array.length != NULL);
    ASSERT(decl->array.length->klass->type == KEFIR_AST_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) decl->array.length->self)->type == KEFIR_AST_INT_CONSTANT);
    ASSERT(((struct kefir_ast_constant *) decl->array.length->self)->value.integer == 1);
    ASSERT(!decl->array.static_array);
    ASSERT(kefir_ast_type_qualifier_list_iter(&decl->array.type_qualifiers, NULL) == NULL);

    kefir_bool_t abstract;
    ASSERT_OK(kefir_ast_declarator_is_abstract(decl, &abstract));
    ASSERT(abstract);

    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, decl));
END_CASE

DEFINE_CASE(ast_declarator_construction10, "AST declarators - function construction #1")
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));

    struct kefir_ast_declarator *decl = kefir_ast_declarator_function(&kft_mem,
        kefir_ast_declarator_identifier(&kft_mem, NULL, "func1"));

    ASSERT(decl != NULL);
    ASSERT(decl->klass == KEFIR_AST_DECLARATOR_FUNCTION);
    ASSERT(decl->function.declarator != NULL);
    ASSERT(decl->function.declarator->klass == KEFIR_AST_DECLARATOR_IDENTIFIER);
    ASSERT(decl->function.declarator->identifier != NULL);
    ASSERT(strcmp(decl->function.declarator->identifier, "func1") == 0);
    ASSERT(kefir_list_length(&decl->function.parameters) == 0);
    ASSERT(!decl->function.ellipsis);

    kefir_bool_t abstract;
    ASSERT_OK(kefir_ast_declarator_is_abstract(decl, &abstract));
    ASSERT(!abstract);

    const char *PARAMS[] = {
        "param1", "param2", "param3"
    };
    const kefir_size_t PARAM_COUNT = sizeof(PARAMS) / sizeof(PARAMS[0]);

    for (kefir_size_t i = 0; i < PARAM_COUNT; i++) {
        ASSERT_OK(kefir_list_insert_after(&kft_mem, &decl->function.parameters, kefir_list_tail(&decl->function.parameters),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, &symbols, PARAMS[i]))));
    }

    ASSERT(kefir_list_length(&decl->function.parameters) == PARAM_COUNT);
    const struct kefir_list_entry *iter = kefir_list_head(&decl->function.parameters);
    for (kefir_size_t i = 0; i < PARAM_COUNT; i++) {
        ASSERT(iter != NULL);
        ASSIGN_DECL_CAST(struct kefir_ast_node_base *, param,
            iter->value);
        ASSERT(param != NULL);
        ASSERT(param->klass->type == KEFIR_AST_IDENTIFIER);
        ASSERT(strcmp(((struct kefir_ast_identifier *) param->self)->identifier, PARAMS[i]) == 0);
        kefir_list_next(&iter);
    }

    ASSERT_OK(kefir_ast_declarator_is_abstract(decl, &abstract));
    ASSERT(!abstract);

    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, decl));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE

DEFINE_CASE(ast_declarator_construction11, "AST declarators - function construction #2")
    ASSERT(kefir_ast_declarator_function(&kft_mem, NULL) == NULL);
    ASSERT(kefir_ast_declarator_function(NULL, NULL) == NULL);

    struct kefir_ast_declarator *decl = kefir_ast_declarator_function(&kft_mem,
        kefir_ast_declarator_pointer(&kft_mem, kefir_ast_declarator_identifier(&kft_mem, NULL, NULL)));

    ASSERT(decl != NULL);
    ASSERT(decl->klass == KEFIR_AST_DECLARATOR_FUNCTION);
    ASSERT(decl->function.declarator != NULL);
    ASSERT(decl->function.declarator->klass == KEFIR_AST_DECLARATOR_POINTER);
    ASSERT(kefir_list_length(&decl->function.parameters) == 0);
    ASSERT(!decl->function.ellipsis);

    kefir_bool_t abstract;
    ASSERT_OK(kefir_ast_declarator_is_abstract(decl, &abstract));
    ASSERT(abstract);

    decl->function.ellipsis = true;

    ASSERT_OK(kefir_ast_declarator_is_abstract(decl, &abstract));
    ASSERT(abstract);
        
    ASSERT_OK(kefir_ast_declarator_free(&kft_mem, decl));
END_CASE
