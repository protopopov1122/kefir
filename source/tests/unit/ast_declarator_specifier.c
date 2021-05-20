#include "kefir/ast/declarator.h"
#include "kefir/ast/node.h"
#include "kefir/test/unit_test.h"
#include <string.h>

DEFINE_CASE(ast_declarator_specifier_construction1, "AST declarator specifiers - type specifier construction #1")
#define ASSERT_TYPE_SPECIFIER(_id, _spec) \
    do { \
        struct kefir_ast_declarator_specifier *specifier = kefir_ast_type_specifier_##_id(&kft_mem); \
        ASSERT(specifier != NULL); \
        ASSERT(specifier->klass == KEFIR_AST_TYPE_SPECIFIER); \
        ASSERT(specifier->type_specifier.specifier == (_spec)); \
        ASSERT_OK(kefir_ast_declarator_specifier_free(&kft_mem, specifier)); \
    } while (0)

    ASSERT_TYPE_SPECIFIER(void, KEFIR_AST_TYPE_SPECIFIER_VOID);
    ASSERT_TYPE_SPECIFIER(char, KEFIR_AST_TYPE_SPECIFIER_CHAR);
    ASSERT_TYPE_SPECIFIER(short, KEFIR_AST_TYPE_SPECIFIER_SHORT);
    ASSERT_TYPE_SPECIFIER(int, KEFIR_AST_TYPE_SPECIFIER_INT);
    ASSERT_TYPE_SPECIFIER(long, KEFIR_AST_TYPE_SPECIFIER_LONG);
    ASSERT_TYPE_SPECIFIER(float, KEFIR_AST_TYPE_SPECIFIER_FLOAT);
    ASSERT_TYPE_SPECIFIER(double, KEFIR_AST_TYPE_SPECIFIER_DOUBLE);
    ASSERT_TYPE_SPECIFIER(signed, KEFIR_AST_TYPE_SPECIFIER_SIGNED);
    ASSERT_TYPE_SPECIFIER(unsigned, KEFIR_AST_TYPE_SPECIFIER_UNSIGNED);
    ASSERT_TYPE_SPECIFIER(bool, KEFIR_AST_TYPE_SPECIFIER_BOOL);
    ASSERT_TYPE_SPECIFIER(complex, KEFIR_AST_TYPE_SPECIFIER_COMPLEX);

#undef ASSERT_TYPE_SPECIFIER
END_CASE

DEFINE_CASE(ast_declarator_specifier_construction2, "AST declarator specifiers - atomic type specifier construction")
    for (int i = -100; i < 100; i++) {
        struct kefir_ast_declarator_specifier *specifier = kefir_ast_type_specifier_atomic(&kft_mem, KEFIR_AST_NODE_BASE(
            kefir_ast_new_constant_int(&kft_mem, i)));
        ASSERT(specifier != NULL);
        ASSERT(specifier->klass == KEFIR_AST_TYPE_SPECIFIER);
        ASSERT(specifier->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_ATOMIC);
        ASSERT(specifier->type_specifier.value.atomic_type->klass->type == KEFIR_AST_CONSTANT);
        ASSERT(specifier->type_specifier.value.atomic_type != NULL);
        ASSERT(((struct kefir_ast_constant *) specifier->type_specifier.value.atomic_type->self)->type == KEFIR_AST_INT_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) specifier->type_specifier.value.atomic_type->self)->value.integer == i);
        ASSERT_OK(kefir_ast_declarator_specifier_free(&kft_mem, specifier));
    }
END_CASE

DEFINE_CASE(ast_declarator_specifier_construction3, "AST declarator specifiers - structure type specifier construction")
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));

    struct kefir_ast_structure_specifier *structure_specifier = kefir_ast_structure_specifier_init(&kft_mem,
        &symbols, "struct1", false);
    struct kefir_ast_declarator_specifier *specifier = kefir_ast_type_specifier_struct(&kft_mem, structure_specifier);
    ASSERT(specifier != NULL);
    ASSERT(specifier->klass == KEFIR_AST_TYPE_SPECIFIER);
    ASSERT(specifier->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_STRUCT);
    ASSERT(specifier->type_specifier.value.structure == structure_specifier);
    ASSERT_OK(kefir_ast_declarator_specifier_free(&kft_mem, specifier));

    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE

DEFINE_CASE(ast_declarator_specifier_construction4, "AST declarator specifiers - union type specifier construction")
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));

    struct kefir_ast_structure_specifier *structure_specifier = kefir_ast_structure_specifier_init(&kft_mem,
        &symbols, "union1", false);

    struct kefir_ast_declarator_specifier *specifier = kefir_ast_type_specifier_union(&kft_mem, structure_specifier);
    ASSERT(specifier != NULL);
    ASSERT(specifier->klass == KEFIR_AST_TYPE_SPECIFIER);
    ASSERT(specifier->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_UNION);
    ASSERT(specifier->type_specifier.value.structure == structure_specifier);
    ASSERT_OK(kefir_ast_declarator_specifier_free(&kft_mem, specifier));

    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE

DEFINE_CASE(ast_declarator_specifier_construction5, "AST declarator specifiers - enumeration type specifier construction")
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));

    struct kefir_ast_enum_specifier *enum_specifier = kefir_ast_enum_specifier_init(&kft_mem,
        &symbols, "enum1", false);
    struct kefir_ast_declarator_specifier *specifier = kefir_ast_type_specifier_enum(&kft_mem, enum_specifier);
    ASSERT(specifier != NULL);
    ASSERT(specifier->klass == KEFIR_AST_TYPE_SPECIFIER);
    ASSERT(specifier->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_ENUM);
    ASSERT(specifier->type_specifier.value.enumeration == enum_specifier);
    ASSERT_OK(kefir_ast_declarator_specifier_free(&kft_mem, specifier));

    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE

DEFINE_CASE(ast_declarator_specifier_construction6, "AST declarator specifiers - typedef type specifier construction #1")
    const char *TYPE_NAMES[] = {
        "",
        "Hello_world",
        "Goodbye_cruel_world",
        "one_two_THREE",
        "A",
        "some_pretty_long_type_name_1"
    };
    const kefir_size_t TYPE_NAME_COUNT = sizeof(TYPE_NAMES) / sizeof(TYPE_NAMES[0]);

    for (kefir_size_t i = 0; i < TYPE_NAME_COUNT; i++) {
        struct kefir_ast_declarator_specifier *specifier = kefir_ast_type_specifier_typedef(&kft_mem, NULL, TYPE_NAMES[i]);
        ASSERT(specifier != NULL);
        ASSERT(specifier->klass == KEFIR_AST_TYPE_SPECIFIER);
        ASSERT(specifier->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_TYPEDEF);
        ASSERT(specifier->type_specifier.value.type_name == TYPE_NAMES[i]);
        ASSERT(strcmp(specifier->type_specifier.value.type_name, TYPE_NAMES[i]) == 0);
        ASSERT_OK(kefir_ast_declarator_specifier_free(&kft_mem, specifier));
    }
END_CASE

DEFINE_CASE(ast_declarator_specifier_construction7, "AST declarator specifiers - typedef type specifier construction #2")
    const char *TYPE_NAMES[] = {
        "",
        "Hello_world",
        "Goodbye_cruel_world",
        "one_two_THREE",
        "A",
        "some_pretty_long_type_name_1"
    };
    const kefir_size_t TYPE_NAME_COUNT = sizeof(TYPE_NAMES) / sizeof(TYPE_NAMES[0]);
    struct kefir_symbol_table symbols;
    ASSERT_OK(kefir_symbol_table_init(&symbols));

    for (kefir_size_t i = 0; i < TYPE_NAME_COUNT; i++) {
        struct kefir_ast_declarator_specifier *specifier = kefir_ast_type_specifier_typedef(&kft_mem, &symbols, TYPE_NAMES[i]);
        ASSERT(specifier != NULL);
        ASSERT(specifier->klass == KEFIR_AST_TYPE_SPECIFIER);
        ASSERT(specifier->type_specifier.specifier == KEFIR_AST_TYPE_SPECIFIER_TYPEDEF);
        ASSERT(specifier->type_specifier.value.type_name != NULL);
        ASSERT(specifier->type_specifier.value.type_name != TYPE_NAMES[i]);
        ASSERT(strcmp(specifier->type_specifier.value.type_name, TYPE_NAMES[i]) == 0);
        ASSERT_OK(kefir_ast_declarator_specifier_free(&kft_mem, specifier));
    }

    ASSERT_OK(kefir_symbol_table_free(&kft_mem, &symbols));
END_CASE

DEFINE_CASE(ast_declarator_specifier_construction8, "AST declarator specifiers - storage class specifier construction")
#define ASSERT_STORAGE_CLASS(_id, _spec) \
    do { \
        struct kefir_ast_declarator_specifier *specifier = kefir_ast_storage_class_specifier_##_id(&kft_mem); \
        ASSERT(specifier != NULL); \
        ASSERT(specifier->klass == KEFIR_AST_STORAGE_CLASS_SPECIFIER); \
        ASSERT(specifier->storage_class == (_spec)); \
        ASSERT_OK(kefir_ast_declarator_specifier_free(&kft_mem, specifier)); \
    } while (0)

    ASSERT_STORAGE_CLASS(typedef, KEFIR_AST_STORAGE_SPECIFIER_TYPEDEF);
    ASSERT_STORAGE_CLASS(extern, KEFIR_AST_STORAGE_SPECIFIER_EXTERN);
    ASSERT_STORAGE_CLASS(static, KEFIR_AST_STORAGE_SPECIFIER_STATIC);
    ASSERT_STORAGE_CLASS(thread_local, KEFIR_AST_STORAGE_SPECIFIER_THREAD_LOCAL);
    ASSERT_STORAGE_CLASS(auto, KEFIR_AST_STORAGE_SPECIFIER_AUTO);
    ASSERT_STORAGE_CLASS(register, KEFIR_AST_STORAGE_SPECIFIER_REGISTER);

#undef ASSERT_STORAGE_CLASS
END_CASE

DEFINE_CASE(ast_declarator_specifier_construction9, "AST declarator specifiers - type qualifier construction")
#define ASSERT_TYPE_QUALIFIER(_id, _spec) \
    do { \
        struct kefir_ast_declarator_specifier *specifier = kefir_ast_type_qualifier_##_id(&kft_mem); \
        ASSERT(specifier != NULL); \
        ASSERT(specifier->klass == KEFIR_AST_TYPE_QUALIFIER); \
        ASSERT(specifier->type_qualifier == (_spec)); \
        ASSERT_OK(kefir_ast_declarator_specifier_free(&kft_mem, specifier)); \
    } while (0)

    ASSERT_TYPE_QUALIFIER(const, KEFIR_AST_TYPE_QUALIFIER_CONST);
    ASSERT_TYPE_QUALIFIER(restrict, KEFIR_AST_TYPE_QUALIFIER_RESTRICT);
    ASSERT_TYPE_QUALIFIER(volatile, KEFIR_AST_TYPE_QUALIFIER_VOLATILE);
    ASSERT_TYPE_QUALIFIER(atomic, KEFIR_AST_TYPE_QUALIFIER_ATOMIC);

#undef ASSERT_TYPE_QUALIFIER
END_CASE

DEFINE_CASE(ast_declarator_specifier_construction10, "AST declarator specifiers - function specifier construction")
#define ASSERT_FUNCTION_SPECIFIER(_id, _spec) \
    do { \
        struct kefir_ast_declarator_specifier *specifier = kefir_ast_function_specifier_##_id(&kft_mem); \
        ASSERT(specifier != NULL); \
        ASSERT(specifier->klass == KEFIR_AST_FUNCTION_SPECIFIER); \
        ASSERT(specifier->function_specifier == (_spec)); \
        ASSERT_OK(kefir_ast_declarator_specifier_free(&kft_mem, specifier)); \
    } while (0)

    ASSERT_FUNCTION_SPECIFIER(inline, KEFIR_AST_FUNCTION_SPECIFIER_TYPE_INLINE);
    ASSERT_FUNCTION_SPECIFIER(noreturn, KEFIR_AST_FUNCTION_SPECIFIER_TYPE_NORETURN);

#undef ASSERT_FUNCTION_SPECIFIER
END_CASE

DEFINE_CASE(ast_declarator_specifier_construction11, "AST declarator specifiers - alignment specifier construction")
    for (int i = 0; i < 100; i++) {
        struct kefir_ast_declarator_specifier *specifier = kefir_ast_alignment_specifier(&kft_mem, KEFIR_AST_NODE_BASE(
            kefir_ast_new_constant_int(&kft_mem, i)));
        ASSERT(specifier != NULL);
        ASSERT(specifier->klass == KEFIR_AST_ALIGNMENT_SPECIFIER);
        ASSERT(specifier->alignment_specifier != NULL);
        ASSERT(specifier->alignment_specifier->klass->type == KEFIR_AST_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) specifier->alignment_specifier->self)->type == KEFIR_AST_INT_CONSTANT);
        ASSERT(((struct kefir_ast_constant *) specifier->alignment_specifier->self)->value.integer == i);
        ASSERT_OK(kefir_ast_declarator_specifier_free(&kft_mem, specifier));
    }
END_CASE
