#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/initializer.h"
#include "kefir/ast/analyzer/initializer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/node.h"

DEFINE_CASE(ast_initializer_analysis1, "AST initializer - analysis #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *TYPES[] = {
        kefir_ast_type_bool(),
        kefir_ast_type_char(),
        kefir_ast_type_signed_char(),
        kefir_ast_type_unsigned_char(),
        kefir_ast_type_signed_short(),
        kefir_ast_type_unsigned_short(),
        kefir_ast_type_signed_int(),
        kefir_ast_type_unsigned_int(),
        kefir_ast_type_signed_long(),
        kefir_ast_type_unsigned_long(),
        kefir_ast_type_signed_long_long(),
        kefir_ast_type_unsigned_long_long(),
        kefir_ast_type_float(),
        kefir_ast_type_double(),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_char()),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_float()),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle,
            kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_int()))
    };
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);

    struct kefir_ast_initializer *init1 = kefir_ast_new_expression_initializer(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)));
    struct kefir_ast_initializer *init2 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &init2->list, NULL, kefir_ast_new_expression_initializer(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))));
    struct kefir_ast_initializer *init3 = kefir_ast_new_list_initializer(&kft_mem);
    
    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, TYPES[i], init1));
        ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, TYPES[i], init2));
        ASSERT_NOK(kefir_ast_analyze_initializer(&kft_mem, context, TYPES[i], init3));
    }
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init3));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_initializer_analysis2, "AST initializer - analysis #2")
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
        "x", kefir_ast_type_signed_long_long(), NULL));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, structure1,
        "y", kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_char()), NULL));

    struct kefir_ast_initializer *init1 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init1));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init1));

    struct kefir_ast_initializer *init2 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &init2->list, NULL, kefir_ast_new_expression_initializer(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 10)))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init2));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init2));

    struct kefir_ast_initializer *init3 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &init3->list, NULL, kefir_ast_new_expression_initializer(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 11)))));
    ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &init3->list, NULL, kefir_ast_new_expression_initializer(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(&kft_mem, 110000)))));
    ASSERT_OK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init3));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init3));

    struct kefir_ast_initializer *init4 = kefir_ast_new_list_initializer(&kft_mem);
    ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &init4->list, NULL, kefir_ast_new_expression_initializer(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 11)))));
    ASSERT_OK(kefir_ast_initializer_list_append(&kft_mem, &init4->list, NULL, kefir_ast_new_expression_initializer(&kft_mem,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(&kft_mem, 3.14)))));
    ASSERT_NOK(kefir_ast_analyze_initializer(&kft_mem, context, type1, init4));
    ASSERT_OK(kefir_ast_initializer_free(&kft_mem, init4));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE