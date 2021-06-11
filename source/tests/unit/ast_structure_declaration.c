#include "kefir/test/unit_test.h"
#include "kefir/ast/declarator.h"
#include "kefir/ast/node.h"
#include <string.h>

DEFINE_CASE(ast_structure_declaration1, "AST Declarations - structure declarations #1") {
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));

    struct kefir_ast_structure_specifier *specifier1 =
        kefir_ast_structure_specifier_init(&kft_mem, &symbols, "struct1", false);
    ASSERT(specifier1 != NULL);

    ASSERT(strcmp(specifier1->identifier, "struct1") == 0);
    ASSERT(!specifier1->complete);

    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
    ASSERT(entry1 != NULL);
    ASSERT(!entry1->is_static_assertion);

    ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
                                                         kefir_ast_type_specifier_int(&kft_mem)));
    ASSERT_OK(kefir_ast_structure_declaration_entry_append(
        &kft_mem, entry1, kefir_ast_declarator_identifier(&kft_mem, &symbols, "field1"), NULL));

    struct kefir_ast_declarator_specifier *entry1_specifier = NULL;
    ASSERT(kefir_ast_declarator_specifier_list_iter(&entry1->declaration.specifiers, &entry1_specifier) != NULL);
    ASSERT(entry1_specifier->klass == KEFIR_AST_TYPE_SPECIFIER);
    ASSERT(entry1_specifier->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_INT);

    ASSIGN_DECL_CAST(struct kefir_ast_structure_entry_declarator *, entry1_declarator,
                     kefir_list_head(&entry1->declaration.declarators)->value);
    ASSERT(entry1_declarator->declarator->klass == KEFIR_AST_DECLARATOR_IDENTIFIER);
    ASSERT(strcmp(entry1_declarator->declarator->identifier, "field1") == 0);
    ASSERT(entry1_declarator->bitwidth == NULL);

    ASSERT_NOK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));
    ASSERT_OK(kefir_ast_structure_declaration_entry_free(&kft_mem, entry1));

    ASSERT_OK(kefir_ast_structure_specifier_free(&kft_mem, specifier1));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE

DEFINE_CASE(ast_structure_declaration2, "AST Declarations - structure declarations #2") {
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));

    struct kefir_ast_structure_specifier *specifier1 =
        kefir_ast_structure_specifier_init(&kft_mem, &symbols, "struct_one", true);
    ASSERT(specifier1 != NULL);
    ASSERT(strcmp(specifier1->identifier, "struct_one") == 0);
    ASSERT(specifier1->complete);

    do {
        struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
        ASSERT(entry1 != NULL);
        ASSERT(!entry1->is_static_assertion);

        ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry1->declaration.specifiers,
                                                             kefir_ast_type_specifier_long(&kft_mem)));
        ASSERT_OK(kefir_ast_structure_declaration_entry_append(
            &kft_mem, entry1, kefir_ast_declarator_identifier(&kft_mem, &symbols, "a"), NULL));
        ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry1));
    } while (0);

    do {
        struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
        ASSERT(entry2 != NULL);
        ASSERT(!entry2->is_static_assertion);

        ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
                                                             kefir_ast_type_specifier_long(&kft_mem)));
        ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry2->declaration.specifiers,
                                                             kefir_ast_type_specifier_long(&kft_mem)));
        ASSERT_OK(kefir_ast_structure_declaration_entry_append(
            &kft_mem, entry2, kefir_ast_declarator_identifier(&kft_mem, &symbols, "b"), NULL));
        ASSERT_OK(kefir_ast_structure_declaration_entry_append(
            &kft_mem, entry2, kefir_ast_declarator_identifier(&kft_mem, &symbols, "c"),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 10))));
        ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry2));
    } while (0);

    do {
        struct kefir_ast_structure_declaration_entry *entry3 = kefir_ast_structure_declaration_entry_alloc_assert(
            &kft_mem,
            kefir_ast_new_static_assertion(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                                           KEFIR_AST_MAKE_STRING_LITERAL(&kft_mem, "Not going to happend")));
        ASSERT(entry3 != NULL);
        ASSERT(entry3->is_static_assertion);
        ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry3));
    } while (0);

    do {
        struct kefir_ast_structure_declaration_entry *entry4 = kefir_ast_structure_declaration_entry_alloc(&kft_mem);
        ASSERT(entry4 != NULL);
        ASSERT(!entry4->is_static_assertion);

        ASSERT_OK(kefir_ast_declarator_specifier_list_append(&kft_mem, &entry4->declaration.specifiers,
                                                             kefir_ast_type_specifier_float(&kft_mem)));
        ASSERT_OK(kefir_ast_structure_declaration_entry_append(
            &kft_mem, entry4,
            kefir_ast_declarator_array(&kft_mem, KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
                                       KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 5)),
                                       kefir_ast_declarator_identifier(&kft_mem, &symbols, "last")),
            NULL));
        ASSERT_OK(kefir_ast_structure_specifier_append_entry(&kft_mem, specifier1, entry4));
    } while (0);

    const struct kefir_list_entry *entry_iter = kefir_list_head(&specifier1->entries);

    do {
        ASSERT(entry_iter != NULL);
        ASSIGN_DECL_CAST(struct kefir_ast_structure_declaration_entry *, entry, entry_iter->value);
        ASSERT(!entry->is_static_assertion);

        struct kefir_ast_declarator_specifier *entry_specifier1 = NULL;
        const struct kefir_list_entry *specifier_iter =
            kefir_ast_declarator_specifier_list_iter(&entry->declaration.specifiers, &entry_specifier1);
        ASSERT(specifier_iter != NULL);
        ASSERT(entry_specifier1->klass == KEFIR_AST_TYPE_SPECIFIER);
        ASSERT(entry_specifier1->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_LONG);
        ASSERT(specifier_iter->next == NULL);

        const struct kefir_list_entry *declarator_iter = kefir_list_head(&entry->declaration.declarators);
        ASSIGN_DECL_CAST(struct kefir_ast_structure_entry_declarator *, entry_declarator, declarator_iter->value);
        ASSERT(declarator_iter != NULL);
        ASSERT(entry_declarator->declarator != NULL);
        ASSERT(entry_declarator->bitwidth == NULL);
        ASSERT(entry_declarator->declarator->klass == KEFIR_AST_DECLARATOR_IDENTIFIER);
        ASSERT(strcmp(entry_declarator->declarator->identifier, "a") == 0);
        kefir_list_next(&declarator_iter);
    } while (0);

    do {
        kefir_list_next(&entry_iter);
        ASSERT(entry_iter != NULL);
        ASSIGN_DECL_CAST(struct kefir_ast_structure_declaration_entry *, entry, entry_iter->value);
        ASSERT(!entry->is_static_assertion);

        struct kefir_ast_declarator_specifier *entry_specifier1 = NULL;
        struct kefir_list_entry *specifier_iter =
            kefir_ast_declarator_specifier_list_iter(&entry->declaration.specifiers, &entry_specifier1);
        ASSERT(specifier_iter != NULL);
        ASSERT(entry_specifier1->klass == KEFIR_AST_TYPE_SPECIFIER);
        ASSERT(entry_specifier1->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_LONG);
        kefir_ast_declarator_specifier_list_next(&specifier_iter, &entry_specifier1);
        ASSERT(specifier_iter != NULL);
        ASSERT(entry_specifier1->klass == KEFIR_AST_TYPE_SPECIFIER);
        ASSERT(entry_specifier1->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_LONG);
        ASSERT(specifier_iter->next == NULL);

        const struct kefir_list_entry *declarator_iter = kefir_list_head(&entry->declaration.declarators);
        ASSIGN_DECL_CAST(struct kefir_ast_structure_entry_declarator *, entry_declarator, declarator_iter->value);
        ASSERT(declarator_iter != NULL);
        ASSERT(entry_declarator->declarator != NULL);
        ASSERT(entry_declarator->bitwidth == NULL);
        ASSERT(entry_declarator->declarator->klass == KEFIR_AST_DECLARATOR_IDENTIFIER);
        ASSERT(strcmp(entry_declarator->declarator->identifier, "b") == 0);
        kefir_list_next(&declarator_iter);
        ASSERT(declarator_iter != NULL);
        entry_declarator = declarator_iter->value;
        ASSERT(entry_declarator->declarator != NULL);
        ASSERT(entry_declarator->declarator->klass == KEFIR_AST_DECLARATOR_IDENTIFIER);
        ASSERT(strcmp(entry_declarator->declarator->identifier, "c") == 0);
        ASSERT(entry_declarator->bitwidth != NULL);
        ASSERT(entry_declarator->bitwidth->klass->type == KEFIR_AST_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) entry_declarator->bitwidth->self)->type == KEFIR_AST_INT_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) entry_declarator->bitwidth->self)->value.integer == 10);
        ASSERT(declarator_iter->next == NULL);
    } while (0);

    do {
        kefir_list_next(&entry_iter);
        ASSERT(entry_iter != NULL);
        ASSIGN_DECL_CAST(struct kefir_ast_structure_declaration_entry *, entry, entry_iter->value);
        ASSERT(entry->is_static_assertion);
        ASSERT(entry->static_assertion != NULL);
        ASSERT(entry->static_assertion->condition->klass->type == KEFIR_AST_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) entry->static_assertion->condition->self)->type ==
               KEFIR_AST_BOOL_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) entry->static_assertion->condition->self)->value.boolean);
    } while (0);

    do {
        kefir_list_next(&entry_iter);
        ASSERT(entry_iter != NULL);
        ASSIGN_DECL_CAST(struct kefir_ast_structure_declaration_entry *, entry, entry_iter->value);
        ASSERT(!entry->is_static_assertion);

        struct kefir_ast_declarator_specifier *entry_specifier1 = NULL;
        const struct kefir_list_entry *specifier_iter =
            kefir_ast_declarator_specifier_list_iter(&entry->declaration.specifiers, &entry_specifier1);
        ASSERT(specifier_iter != NULL);
        ASSERT(entry_specifier1->klass == KEFIR_AST_TYPE_SPECIFIER);
        ASSERT(entry_specifier1->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_FLOAT);
        ASSERT(specifier_iter->next == NULL);

        const struct kefir_list_entry *declarator_iter = kefir_list_head(&entry->declaration.declarators);
        ASSIGN_DECL_CAST(struct kefir_ast_structure_entry_declarator *, entry_declarator, declarator_iter->value);
        ASSERT(declarator_iter != NULL);
        ASSERT(entry_declarator->declarator != NULL);
        ASSERT(entry_declarator->bitwidth == NULL);
        ASSERT(entry_declarator->declarator->klass == KEFIR_AST_DECLARATOR_ARRAY);
        ASSERT(!entry_declarator->declarator->array.static_array);
        ASSERT(kefir_ast_type_qualifier_list_iter(&entry_declarator->declarator->array.type_qualifiers, NULL) == NULL);
        ASSERT(entry_declarator->declarator->array.length->klass->type == KEFIR_AST_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) entry_declarator->declarator->array.length->self)->type ==
               KEFIR_AST_INT_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) entry_declarator->declarator->array.length->self)->value.character == 5);
        ASSERT(entry_declarator->declarator->array.declarator->klass == KEFIR_AST_DECLARATOR_IDENTIFIER);
        ASSERT(strcmp(entry_declarator->declarator->array.declarator->identifier, "last") == 0);
        kefir_list_next(&declarator_iter);
    } while (0);

    kefir_list_next(&entry_iter);
    ASSERT(entry_iter == NULL);

    ASSERT_OK(kefir_ast_structure_specifier_free(&kft_mem, specifier1));
    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
}
END_CASE
