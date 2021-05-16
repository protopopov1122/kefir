#include <string.h>
#include "kefir/test/unit_test.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/type_conv.h"
#include "kefir/test/util.h"

struct kefir_ast_constant *make_constant(struct kefir_mem *, const struct kefir_ast_type *);

#define ASSERT_CONDITIONAL(_mem, _context, _cond, _expr1, _expr2, _const, _checker) \
    do { \
        struct kefir_ast_conditional_operator *oper = kefir_ast_new_conditional_operator((_mem), \
            (_cond), (_expr1), (_expr2)); \
        ASSERT(oper != NULL); \
        ASSERT_OK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(oper))); \
        ASSERT(oper->base.properties.category == KEFIR_AST_NODE_CATEGORY_EXPRESSION); \
        ASSERT(oper->base.properties.expression_props.constant_expression == (_const)); \
        ASSERT(!oper->base.properties.expression_props.lvalue); \
        ASSERT(!oper->base.properties.expression_props.addressable); \
        ASSERT(!oper->base.properties.expression_props.bitfield); \
        _checker; \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(oper))); \
    } while (0)

#define ASSERT_CONDITIONAL_NOK(_mem, _context, _cond, _expr1, _expr2) \
    do { \
        struct kefir_ast_conditional_operator *oper = kefir_ast_new_conditional_operator((_mem), \
            (_cond), \
            (_expr1), \
            (_expr2)); \
        ASSERT(oper != NULL); \
        ASSERT_NOK(kefir_ast_analyze_node((_mem), (_context), KEFIR_AST_NODE_BASE(oper))); \
        ASSERT_OK(KEFIR_AST_NODE_FREE((_mem), KEFIR_AST_NODE_BASE(oper))); \
    } while (0)

DEFINE_CASE(ast_node_analysis_conditional_operator1, "AST node analysis - conditional operator #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem,
        context->type_bundle, "", &struct_type1);

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context,
        "x", type1, NULL));
    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context,
        "y", kefir_ast_type_unsigned_int(), NULL));
    ASSERT_OK(kefir_ast_local_context_define_constant(&kft_mem, &local_context, "X",
        kefir_ast_constant_expression_integer(&kft_mem, 101), type_traits->underlying_enumeration_type));

    const struct kefir_ast_type *TYPES[] = {
        kefir_ast_type_bool(),
        kefir_ast_type_unsigned_char(),
        kefir_ast_type_unsigned_int(),
        kefir_ast_type_signed_int(),
        kefir_ast_type_unsigned_long(),
        kefir_ast_type_signed_long(),
        kefir_ast_type_unsigned_long_long(),
        kefir_ast_type_signed_long_long(),
        kefir_ast_type_float(),
        kefir_ast_type_double()
    };
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);

    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        ASSERT_CONDITIONAL(&kft_mem, context,
            KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)),
            true, {
            ASSERT(KEFIR_AST_TYPE_SAME(oper->condition->properties.type, TYPES[i]));
        });

        ASSERT_CONDITIONAL(&kft_mem, context,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
                kefir_ast_type_pointer(&kft_mem, context->type_bundle, TYPES[i]),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)),
            true, {
            ASSERT(KEFIR_AST_TYPE_SAME(oper->condition->properties.type,
                kefir_ast_type_pointer(&kft_mem, context->type_bundle, TYPES[i])));
        });
    }

    ASSERT_CONDITIONAL_NOK(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)));

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "y")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)),
        false, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->condition->properties.type, kefir_ast_type_unsigned_int()));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "X")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 1)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 2)),
        true, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->condition->properties.type, type_traits->underlying_enumeration_type));
    });

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_conditional_operator2, "AST node analysis - conditional operator #2")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_OK(kefir_ast_global_context_declare_external(&kft_mem, &global_context,
        "x", kefir_ast_type_signed_int(), NULL));
    ASSERT_OK(kefir_ast_global_context_define_constant(&kft_mem, &global_context,
        "X", kefir_ast_constant_expression_integer(&kft_mem, 54), type_traits->underlying_enumeration_type));

    const struct kefir_ast_type *TYPES[] = {
        kefir_ast_type_bool(),
        kefir_ast_type_char(),
        kefir_ast_type_unsigned_char(),
        kefir_ast_type_signed_char(),
        kefir_ast_type_unsigned_int(),
        kefir_ast_type_signed_int(),
        kefir_ast_type_unsigned_long(),
        kefir_ast_type_signed_long(),
        kefir_ast_type_unsigned_long_long(),
        kefir_ast_type_signed_long_long(),
        kefir_ast_type_float(),
        kefir_ast_type_double()
    };
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);
    
    for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
        for (kefir_size_t j = 0; j < TYPES_LEN; j++) {
            ASSERT_CONDITIONAL(&kft_mem, context,
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
                KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])),
                KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[j])),
                true, {
                ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type,
                    kefir_ast_type_common_arithmetic(context->type_traits, TYPES[i], TYPES[j])));
            });
        }

        ASSERT_CONDITIONAL(&kft_mem, context,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
            KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "x")),
            false, {
            ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type,
                kefir_ast_type_common_arithmetic(context->type_traits, TYPES[i], kefir_ast_type_signed_int())));
        });

        ASSERT_CONDITIONAL(&kft_mem, context,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
            KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])),
            KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "X")),
            true, {
            ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type,
                kefir_ast_type_common_arithmetic(context->type_traits, TYPES[i], type_traits->underlying_enumeration_type)));
        });

        ASSERT_CONDITIONAL_NOK(&kft_mem, context,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
                    kefir_ast_type_void(),
                    KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
            KEFIR_AST_NODE_BASE(make_constant(&kft_mem, TYPES[i])));
    }

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
                kefir_ast_type_void(),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
                kefir_ast_type_void(),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, kefir_ast_type_void()));
    });

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_conditional_operator3, "AST node analysis - conditional operator #3")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_structure(&kft_mem,
        context->type_bundle, "", &struct_type1);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type1,
        "x", kefir_ast_type_signed_int(), NULL));

    struct kefir_ast_struct_type *struct_type2 = NULL;
    const struct kefir_ast_type *type2 = kefir_ast_type_structure(&kft_mem,
        context->type_bundle, "", &struct_type2);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type2,
        "y", kefir_ast_type_unsigned_long(), NULL));

    struct kefir_ast_struct_type *struct_type3 = NULL;
    const struct kefir_ast_type *type3 = kefir_ast_type_structure(&kft_mem,
        context->type_bundle, "", &struct_type3);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, struct_type3,
        "z", kefir_ast_type_double(), NULL));

    struct kefir_ast_struct_type *union_type4 = NULL;
    const struct kefir_ast_type *type4 = kefir_ast_type_union(&kft_mem,
        context->type_bundle, "", &union_type4);
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context->symbols, union_type4,
        "z", kefir_ast_type_double(), NULL));

    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &local_context,
        "a1", type1, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &local_context,
        "a2", type1, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &local_context,
        "b1", type2, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &local_context,
        "b2", type2, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &local_context,
        "c1", type3, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &local_context,
        "c2", type3, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_auto(&kft_mem, &local_context,
        "d1", type4, NULL, NULL));
    ASSERT_OK(kefir_ast_local_context_define_static(&kft_mem, &local_context,
        "d2", type4, NULL, NULL));

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a1")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a1")),
        false, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type1));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a1")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a2")),
        false, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type1));
    });

    ASSERT_CONDITIONAL_NOK(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a1")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b1")));

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b1")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b1")),
        false, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type2));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b1")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b2")),
        false, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type2));
    });

    ASSERT_CONDITIONAL_NOK(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "b2")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c1")));

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c1")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c1")),
        false, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type3));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c1")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c2")),
        false, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type3));
    });

    ASSERT_CONDITIONAL_NOK(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c1")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "a2")));

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d1")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d1")),
        false, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type4));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d1")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d2")),
        false, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type4));
    });

    ASSERT_CONDITIONAL_NOK(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "d1")),
        KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(&kft_mem, context->symbols, "c1")));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_conditional_operator4, "AST node analysis - conditional operator #4")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *type1 =
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_signed_long());

    const struct kefir_ast_type *type2 =
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_qualified(&kft_mem, context->type_bundle,
            kefir_ast_type_signed_long(), (struct kefir_ast_type_qualification) {
                .constant = true,
                .restricted = false,
                .volatile_type = false
            }));

    const struct kefir_ast_type *type3 =
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_qualified(&kft_mem, context->type_bundle,
            kefir_ast_type_signed_long(), (struct kefir_ast_type_qualification) {
                .constant = false,
                .restricted = true,
                .volatile_type = false
            }));

    const struct kefir_ast_type *type4 =
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_qualified(&kft_mem, context->type_bundle,
            kefir_ast_type_signed_long(), (struct kefir_ast_type_qualification) {
                .constant = true,
                .restricted = true,
                .volatile_type = false
            }));

    const struct kefir_ast_type *type5 =
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void());


    const struct kefir_ast_type *type6 =
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_qualified(&kft_mem, context->type_bundle,
            kefir_ast_type_void(), (struct kefir_ast_type_qualification) {
                .constant = true,
                .restricted = false,
                .volatile_type = false
            }));

    const struct kefir_ast_type *type7 =
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_qualified(&kft_mem, context->type_bundle,
            kefir_ast_type_void(), (struct kefir_ast_type_qualification) {
                .constant = false,
                .restricted = true,
                .volatile_type = false
            }));

    const struct kefir_ast_type *type8 =
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_qualified(&kft_mem, context->type_bundle,
            kefir_ast_type_void(), (struct kefir_ast_type_qualification) {
                .constant = true,
                .restricted = true,
                .volatile_type = false
            }));

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type1));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type2,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type2));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type2,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type3,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type4));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type2,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type4,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type4));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type3,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type3));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type3,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type2,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type4));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type5,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type5));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type1,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type6,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type6));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type2,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type6,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type6));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type3,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type6,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type8));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type3,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type7,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type7));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type5,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type5,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type5));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type6,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type5,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type6));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type6,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type7,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type8));
    });

    ASSERT_CONDITIONAL(&kft_mem, context,
        KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type4,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
            type8,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
        true, {
        ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, type8));
    });

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_node_analysis_conditional_operator5, "AST node analysis - conditional operator #5")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    const struct kefir_ast_type *TYPES[] = {
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_signed_long()),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_qualified(&kft_mem, context->type_bundle,
            kefir_ast_type_signed_long(), (struct kefir_ast_type_qualification) {
                .constant = true,
                .restricted = false,
                .volatile_type = false
            })),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_qualified(&kft_mem, context->type_bundle,
            kefir_ast_type_signed_long(), (struct kefir_ast_type_qualification) {
                .constant = false,
                .restricted = true,
                .volatile_type = false
            })),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_qualified(&kft_mem, context->type_bundle,
            kefir_ast_type_signed_long(), (struct kefir_ast_type_qualification) {
                .constant = true,
                .restricted = true,
                .volatile_type = false
            })),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_void()),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_qualified(&kft_mem, context->type_bundle,
            kefir_ast_type_void(), (struct kefir_ast_type_qualification) {
                .constant = true,
                .restricted = false,
                .volatile_type = false
            })),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_qualified(&kft_mem, context->type_bundle,
            kefir_ast_type_void(), (struct kefir_ast_type_qualification) {
                .constant = false,
                .restricted = true,
                .volatile_type = false
            })),
        kefir_ast_type_pointer(&kft_mem, context->type_bundle, kefir_ast_type_qualified(&kft_mem, context->type_bundle,
            kefir_ast_type_void(), (struct kefir_ast_type_qualification) {
                .constant = true,
                .restricted = true,
                .volatile_type = false
            }))
    };
    const kefir_size_t TYPE_LENGTH = sizeof(TYPES) / sizeof(TYPES[0]);

    for (kefir_size_t i = 0; i < TYPE_LENGTH; i++) {
        ASSERT_CONDITIONAL(&kft_mem, context,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
                TYPES[i],
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)),
            true, {
            ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, TYPES[i]));
        });
        ASSERT_CONDITIONAL(&kft_mem, context,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_bool(&kft_mem, true)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)),
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(&kft_mem,
                TYPES[i],
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(&kft_mem, 0)))),
            true, {
            ASSERT(KEFIR_AST_TYPE_SAME(oper->base.properties.type, TYPES[i]));
        });
    }

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE
