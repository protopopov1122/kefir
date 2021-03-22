#include "kefir/test/unit_test.h"
#include "kefir/ast/analyzer/type_traversal.h"
#include "kefir/ast/type.h"
#include "kefir/ast/local_context.h"

#define ASSERT_NEXT(_mem, _traversal, _type) \
    do { \
        ASSERT(kefir_ast_type_traversal_empty((_traversal)) == (_type == NULL)); \
        const struct kefir_ast_type *type = NULL; \
        ASSERT_OK(kefir_ast_type_traversal_next((_mem), (_traversal), &type)); \
        if ((_type) == NULL) { \
            ASSERT(type == NULL); \
        } else { \
            ASSERT(KEFIR_AST_TYPE_SAME((const struct kefir_ast_type *) (_type), type)); \
        } \
    } while (0)

#define ASSERT_NEXT_REC(_mem, _traversal, _type) \
    do { \
        ASSERT(kefir_ast_type_traversal_empty((_traversal)) == (_type == NULL)); \
        const struct kefir_ast_type *type = NULL; \
        ASSERT_OK(kefir_ast_type_traversal_next_recursive((_mem), (_traversal), &type)); \
        if ((_type) == NULL) { \
            ASSERT(type == NULL); \
        } else { \
            ASSERT(KEFIR_AST_TYPE_SAME((const struct kefir_ast_type *) (_type), type)); \
        } \
    } while (0)

DEFINE_CASE(ast_type_traversal1, "AST Type analysis - type traversal #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *structure1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "", &structure1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1,
        "x", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1,
        "y", kefir_ast_type_char(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1,
        "z", kefir_ast_type_double(), NULL));

    struct kefir_ast_struct_type *structure2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "", &structure2);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure2,
        "a", kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure2,
        "b", type1, NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure2,
        "c", kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure2,
        "d", kefir_ast_type_pointer(&kft_mem, context->type_bundle, type1), NULL));

    struct kefir_ast_struct_type *union3 = NULL;
    const struct kefir_ast_type *type3 = kefir_ast_type_union(&kft_mem, context->type_bundle,
        "", &union3);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union3,
        "j", type2, NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union3,
        "k", type1, NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union3,
        "l", kefir_ast_type_signed_int(), NULL));

    const struct kefir_ast_type *type4 = 
        kefir_ast_type_array(&kft_mem, context->type_bundle, type3, 4, NULL);

    const struct kefir_ast_type *type5 = 
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, type1, NULL);

    struct kefir_ast_type_traversal traversal;
    ASSERT_OK(kefir_ast_type_traversal_init(&kft_mem, &traversal, type1));
    ASSERT_NEXT(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT(&kft_mem, &traversal, kefir_ast_type_char());
    ASSERT_NEXT(&kft_mem, &traversal, kefir_ast_type_double());
    ASSERT_NEXT(&kft_mem, &traversal, NULL);
    ASSERT_OK(kefir_ast_type_traversal_free(&kft_mem, &traversal));

    ASSERT_OK(kefir_ast_type_traversal_init(&kft_mem, &traversal, type2));
    ASSERT_NEXT(&kft_mem, &traversal, kefir_ast_type_float());
    ASSERT_NEXT(&kft_mem, &traversal, type1);
    ASSERT_NEXT(&kft_mem, &traversal, kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()));
    ASSERT_NEXT(&kft_mem, &traversal, kefir_ast_type_pointer(&kft_mem, context->type_bundle, type1));
    ASSERT_NEXT(&kft_mem, &traversal, NULL);
    ASSERT_OK(kefir_ast_type_traversal_free(&kft_mem, &traversal));

    ASSERT_OK(kefir_ast_type_traversal_init(&kft_mem, &traversal, type3));
    ASSERT_NEXT(&kft_mem, &traversal, type2);
    ASSERT_NEXT(&kft_mem, &traversal, NULL);
    ASSERT_OK(kefir_ast_type_traversal_free(&kft_mem, &traversal));

    ASSERT_OK(kefir_ast_type_traversal_init(&kft_mem, &traversal, type4));
    ASSERT_NEXT(&kft_mem, &traversal, type3);
    ASSERT_NEXT(&kft_mem, &traversal, type3);
    ASSERT_NEXT(&kft_mem, &traversal, type3);
    ASSERT_NEXT(&kft_mem, &traversal, type3);
    ASSERT_NEXT(&kft_mem, &traversal, NULL);
    ASSERT_OK(kefir_ast_type_traversal_free(&kft_mem, &traversal));

    ASSERT_OK(kefir_ast_type_traversal_init(&kft_mem, &traversal, type5));
    for (kefir_size_t i = 0; i < 100; i++) {
        ASSERT_NEXT(&kft_mem, &traversal, type1);
    }
    ASSERT_OK(kefir_ast_type_traversal_free(&kft_mem, &traversal));

    ASSERT_OK(kefir_ast_type_traversal_init(&kft_mem, &traversal, kefir_ast_type_signed_short()));
    ASSERT_NEXT(&kft_mem, &traversal, kefir_ast_type_signed_short());
    ASSERT_NEXT(&kft_mem, &traversal, NULL);
    ASSERT_OK(kefir_ast_type_traversal_free(&kft_mem, &traversal));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_type_traversal2, "AST Type analysis - type traversal #2")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *structure1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "", &structure1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1,
        "x", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1,
        "y", kefir_ast_type_char(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1,
        "z", kefir_ast_type_double(), NULL));

    struct kefir_ast_struct_type *structure2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "", &structure2);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure2,
        "a", kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure2,
        "b", type1, NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure2,
        "c", kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure2,
        "d", kefir_ast_type_pointer(&kft_mem, context->type_bundle, type1), NULL));

    struct kefir_ast_struct_type *union3 = NULL;
    const struct kefir_ast_type *type3 = kefir_ast_type_union(&kft_mem, context->type_bundle,
        "", &union3);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union3,
        "j", type2, NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union3,
        "k", type1, NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union3,
        "l", kefir_ast_type_signed_int(), NULL));

    const struct kefir_ast_type *type4 = 
        kefir_ast_type_array(&kft_mem, context->type_bundle, type3, 4, NULL);

    const struct kefir_ast_type *type5 = 
        kefir_ast_type_unbounded_array(&kft_mem, context->type_bundle, type1, NULL);

    struct kefir_ast_type_traversal traversal;
    ASSERT_OK(kefir_ast_type_traversal_init(&kft_mem, &traversal, type1));
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_char());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_double());
    ASSERT_NEXT_REC(&kft_mem, &traversal, NULL);
    ASSERT_OK(kefir_ast_type_traversal_free(&kft_mem, &traversal));

    ASSERT_OK(kefir_ast_type_traversal_init(&kft_mem, &traversal, type2));
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_float());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_char());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_double());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()));
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_pointer(&kft_mem, context->type_bundle, type1));
    ASSERT_NEXT_REC(&kft_mem, &traversal, NULL);
    ASSERT_OK(kefir_ast_type_traversal_free(&kft_mem, &traversal));

    ASSERT_OK(kefir_ast_type_traversal_init(&kft_mem, &traversal, type3));
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_float());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_char());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_double());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()));
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_pointer(&kft_mem, context->type_bundle, type1));
    ASSERT_NEXT_REC(&kft_mem, &traversal, NULL);
    ASSERT_OK(kefir_ast_type_traversal_free(&kft_mem, &traversal));

    ASSERT_OK(kefir_ast_type_traversal_init(&kft_mem, &traversal, type4));
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_float());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_char());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_double());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()));
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_pointer(&kft_mem, context->type_bundle, type1));

    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_float());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_char());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_double());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()));
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_pointer(&kft_mem, context->type_bundle, type1));

    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_float());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_char());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_double());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()));
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_pointer(&kft_mem, context->type_bundle, type1));

    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_float());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_char());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_double());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()));
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_pointer(&kft_mem, context->type_bundle, type1));
    ASSERT_NEXT_REC(&kft_mem, &traversal, NULL);
    ASSERT_OK(kefir_ast_type_traversal_free(&kft_mem, &traversal));

    ASSERT_OK(kefir_ast_type_traversal_init(&kft_mem, &traversal, type5));
    for (kefir_size_t i = 0; i < 100; i++) {
        ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
        ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_char());
        ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_double());
    }
    ASSERT_OK(kefir_ast_type_traversal_free(&kft_mem, &traversal));

    ASSERT_OK(kefir_ast_type_traversal_init(&kft_mem, &traversal, kefir_ast_type_signed_short()));
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_short());
    ASSERT_NEXT_REC(&kft_mem, &traversal, NULL);
    ASSERT_OK(kefir_ast_type_traversal_free(&kft_mem, &traversal));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_type_traversal3, "AST Type analysis - type traversal #3")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *structure1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "", &structure1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1,
        "a", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1,
        "b", kefir_ast_type_char(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1,
        "c", kefir_ast_type_double(), NULL));

    const struct kefir_ast_type *type2 = kefir_ast_type_array(&kft_mem, context->type_bundle,
        type1, 5, NULL);
    
    struct kefir_ast_struct_type *structure3 = NULL;
    const struct kefir_ast_type *type3 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "", &structure3);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure3,
        "d", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure3,
        "e", type2, NULL));
    
    struct kefir_ast_struct_type *union4 = NULL;
    const struct kefir_ast_type *type4 = kefir_ast_type_union(&kft_mem, context->type_bundle,
        "", &union4);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union4,
        "f", type3, NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union4,
        "g", type1, NULL));

    const struct kefir_ast_type *type5 = kefir_ast_type_array(&kft_mem, context->type_bundle,
        type4, 3, NULL);

    struct kefir_ast_struct_type *structure6 = NULL;
    const struct kefir_ast_type *type6 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "", &structure6);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure6,
        "h", kefir_ast_type_float(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure6,
        "i", type5, NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure6,
        "j", type1, NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure6,
        "k", type3, NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure6,
        "l", kefir_ast_type_unsigned_long(), NULL));

    struct kefir_ast_type_traversal traversal;
    ASSERT_OK(kefir_ast_type_traversal_init(&kft_mem, &traversal, type6));
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_float());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT(&kft_mem, &traversal, type2);
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_char());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_double());
    ASSERT_NEXT(&kft_mem, &traversal, type1);
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_char());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_double());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_char());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_double());
    ASSERT_NEXT(&kft_mem, &traversal, type1);
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT(&kft_mem, &traversal, type2);
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_char());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_double());
    ASSERT_NEXT(&kft_mem, &traversal, type3);
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_unsigned_long());
    ASSERT_NEXT_REC(&kft_mem, &traversal, NULL);
    ASSERT_OK(kefir_ast_type_traversal_free(&kft_mem, &traversal));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_type_traversal4, "AST Type analysis - type traversal #4")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *structure1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "", &structure1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1,
        "a", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1,
        "b", kefir_ast_type_signed_long(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1,
        "c", kefir_ast_type_signed_long_long(), NULL));

    const struct kefir_ast_type *type2 = kefir_ast_type_array(&kft_mem, context->type_bundle,
        type1, 100, NULL);

    struct kefir_ast_struct_type *structure3 = NULL;
    const struct kefir_ast_type *type3 = kefir_ast_type_structure(&kft_mem, context->type_bundle,
        "", &structure3);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure3,
        "x", type1, NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure3,
        "y", type2, NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure3,
        "z", type1, NULL));

    struct kefir_ast_type_traversal traversal;
    ASSERT_OK(kefir_ast_type_traversal_init(&kft_mem, &traversal, type3));

    struct kefir_ast_designator *designator1 = kefir_ast_new_member_desginator(&kft_mem, context->symbols,
        "c", kefir_ast_new_index_desginator(&kft_mem, 97, kefir_ast_new_member_desginator(&kft_mem, context->symbols,
            "y", NULL)));
    ASSERT(designator1 != NULL);
    ASSERT(designator1->next != NULL);
    ASSERT(designator1->next->next != NULL);
    ASSERT(designator1->next->next->next == NULL);
    ASSERT_OK(kefir_ast_type_traversal_navigate(&kft_mem, &traversal, designator1));
    ASSERT_OK(kefir_ast_designator_free(&kft_mem, designator1));
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long_long());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long_long());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long_long());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long_long());
    ASSERT_NEXT_REC(&kft_mem, &traversal, NULL);
    
    designator1 = kefir_ast_new_member_desginator(&kft_mem, context->symbols,
        "c", kefir_ast_new_index_desginator(&kft_mem, 98, kefir_ast_new_member_desginator(&kft_mem, context->symbols,
            "y", NULL)));
    ASSERT(designator1 != NULL);
    ASSERT(designator1->next != NULL);
    ASSERT(designator1->next->next != NULL);
    ASSERT(designator1->next->next->next == NULL);
    ASSERT_OK(kefir_ast_type_traversal_navigate(&kft_mem, &traversal, designator1));
    ASSERT_OK(kefir_ast_designator_free(&kft_mem, designator1));
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long_long());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long_long());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long_long());
    ASSERT_NEXT_REC(&kft_mem, &traversal, NULL);

    designator1 = kefir_ast_new_member_desginator(&kft_mem, context->symbols,
        "c", kefir_ast_new_index_desginator(&kft_mem, 99, kefir_ast_new_member_desginator(&kft_mem, context->symbols,
            "y", NULL)));
    ASSERT(designator1 != NULL);
    ASSERT(designator1->next != NULL);
    ASSERT(designator1->next->next != NULL);
    ASSERT(designator1->next->next->next == NULL);
    ASSERT_OK(kefir_ast_type_traversal_navigate(&kft_mem, &traversal, designator1));
    ASSERT_OK(kefir_ast_designator_free(&kft_mem, designator1));
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long_long());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long_long());
    ASSERT_NEXT_REC(&kft_mem, &traversal, NULL);

    designator1 = kefir_ast_new_member_desginator(&kft_mem, context->symbols,
        "z", NULL);
    ASSERT(designator1 != NULL);
    ASSERT(designator1->next == NULL);
    ASSERT_OK(kefir_ast_type_traversal_navigate(&kft_mem, &traversal, designator1));
    ASSERT_OK(kefir_ast_designator_free(&kft_mem, designator1));
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long());
    ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long_long());
    ASSERT_NEXT_REC(&kft_mem, &traversal, NULL);

    designator1 = kefir_ast_new_member_desginator(&kft_mem, context->symbols,
        "x", NULL);
    ASSERT(designator1 != NULL);
    ASSERT(designator1->next == NULL);
    ASSERT_OK(kefir_ast_type_traversal_navigate(&kft_mem, &traversal, designator1));
    ASSERT_OK(kefir_ast_designator_free(&kft_mem, designator1));

    for (kefir_size_t i = 0; i < 102; i++) {
        ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_int());
        ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long());
        ASSERT_NEXT_REC(&kft_mem, &traversal, kefir_ast_type_signed_long_long());
    }
    ASSERT_NEXT_REC(&kft_mem, &traversal, NULL);
    ASSERT_OK(kefir_ast_type_traversal_free(&kft_mem, &traversal));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE