#include "kefir/test/unit_test.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/test/util.h"

DEFINE_CASE(ast_bitfield_translator1, "AST translator - bitfields #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;
    struct kefir_ast_translator_environment env;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));
    ASSERT_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_structure(&kft_mem, context.context.type_bundle, NULL, &struct_type1);
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field1",
                                             kefir_ast_type_signed_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 2)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field2",
                                             kefir_ast_type_signed_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 3)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field3",
                                             kefir_ast_type_signed_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 10)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field4",
                                             kefir_ast_type_signed_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 1)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field5",
                                             kefir_ast_type_signed_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 8)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field6",
                                             kefir_ast_type_signed_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 10)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field7",
                                             kefir_ast_type_signed_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 22)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field8",
                                             kefir_ast_type_signed_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 32)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field9",
                                             kefir_ast_type_signed_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 1)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, NULL,
                                             kefir_ast_type_signed_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 1)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field10",
                                             kefir_ast_type_signed_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 2)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, NULL,
                                             kefir_ast_type_signed_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 0)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field11",
                                             kefir_ast_type_signed_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 1)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, NULL,
                                             kefir_ast_type_char(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 0)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field12",
                                             kefir_ast_type_char(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 1)));

    struct kefir_irbuilder_type type_builder;
    struct kefir_ast_designator designator = {.type = KEFIR_AST_DESIGNATOR_MEMBER, .member = NULL, .next = NULL};
    struct kefir_ast_type_layout *member_layout = NULL;

    struct kefir_ir_type ir_type1;
    struct kefir_ast_type_layout *layout1 = NULL;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 0, &ir_type1));
    ASSERT_OK(kefir_irbuilder_type_init(&kft_mem, &type_builder, &ir_type1));
    ASSERT_OK(kefir_ast_translate_object_type(&kft_mem, type1, 0, &env, &type_builder, &layout1));

#define ASSERT_MEMBER(_id, _pos, _width, _offset)                                                   \
    do {                                                                                            \
        designator.member = (_id);                                                                  \
        ASSERT_OK(kefir_ast_type_layout_resolve(layout1, &designator, &member_layout, NULL, NULL)); \
        ASSERT(member_layout->value == (_pos));                                                     \
        struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(&ir_type1, member_layout->value);   \
        ASSERT(typeentry->typecode == KEFIR_IR_TYPE_BITS);                                          \
        ASSERT(member_layout->bitfield);                                                            \
        ASSERT(member_layout->bitfield_props.width == (_width));                                    \
        ASSERT(member_layout->bitfield_props.offset == (_offset));                                  \
    } while (0)

    ASSERT_MEMBER("field1", 1, 2, 0);
    ASSERT_MEMBER("field2", 1, 3, 2);
    ASSERT_MEMBER("field3", 1, 10, 5);
    ASSERT_MEMBER("field4", 1, 1, 15);
    ASSERT_MEMBER("field5", 1, 8, 16);
    ASSERT_MEMBER("field6", 2, 10, 8);
    ASSERT_MEMBER("field7", 2, 22, 18);
    ASSERT_MEMBER("field8", 2, 32, 40);
    ASSERT_MEMBER("field9", 2, 1, 72);
    ASSERT_MEMBER("field10", 2, 2, 74);
    ASSERT_MEMBER("field11", 3, 1, 0);
    ASSERT_MEMBER("field12", 4, 1, 0);

#undef ASSERT_MEMBER

    ASSERT(kefir_ir_type_total_length(&ir_type1) == 5);

    ASSERT_OK(kefir_ast_type_layout_free(&kft_mem, layout1));
    ASSERT_OK(KEFIR_IRBUILDER_TYPE_FREE(&type_builder));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &ir_type1));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_bitfield_translator2, "AST translator - bitfields #2") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;
    struct kefir_ast_translator_environment env;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));
    ASSERT_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_structure(&kft_mem, context.context.type_bundle, NULL, &struct_type1);
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field1",
                                             kefir_ast_type_unsigned_long(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 5)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field2",
                                             kefir_ast_type_signed_short(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 10)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field3",
                                             kefir_ast_type_signed_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 11)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field4",
                                             kefir_ast_type_unsigned_char(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 8)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field5",
                                             kefir_ast_type_signed_long(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 13)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field6",
                                             kefir_ast_type_unsigned_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 12)));

    struct kefir_irbuilder_type type_builder;
    struct kefir_ast_designator designator = {.type = KEFIR_AST_DESIGNATOR_MEMBER, .member = NULL, .next = NULL};
    struct kefir_ast_type_layout *member_layout = NULL;

    struct kefir_ir_type ir_type1;
    struct kefir_ast_type_layout *layout1 = NULL;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 0, &ir_type1));
    ASSERT_OK(kefir_irbuilder_type_init(&kft_mem, &type_builder, &ir_type1));
    ASSERT_OK(kefir_ast_translate_object_type(&kft_mem, type1, 0, &env, &type_builder, &layout1));

#define ASSERT_MEMBER(_id, _pos, _width, _offset)                                                   \
    do {                                                                                            \
        designator.member = (_id);                                                                  \
        ASSERT_OK(kefir_ast_type_layout_resolve(layout1, &designator, &member_layout, NULL, NULL)); \
        ASSERT(member_layout->value == (_pos));                                                     \
        struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(&ir_type1, member_layout->value);   \
        ASSERT(typeentry->typecode == KEFIR_IR_TYPE_BITS);                                          \
        ASSERT(member_layout->bitfield);                                                            \
        ASSERT(member_layout->bitfield_props.width == (_width));                                    \
        ASSERT(member_layout->bitfield_props.offset == (_offset));                                  \
    } while (0)

    ASSERT_MEMBER("field1", 1, 5, 0);
    ASSERT_MEMBER("field2", 1, 10, 5);
    ASSERT_MEMBER("field3", 1, 11, 15);
    ASSERT_MEMBER("field4", 2, 8, 0);
    ASSERT_MEMBER("field5", 2, 13, 8);
    ASSERT_MEMBER("field6", 3, 12, 8);

#undef ASSERT_MEMBER

    ASSERT(kefir_ir_type_total_length(&ir_type1) == 4);

    ASSERT_OK(kefir_ast_type_layout_free(&kft_mem, layout1));
    ASSERT_OK(KEFIR_IRBUILDER_TYPE_FREE(&type_builder));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &ir_type1));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_bitfield_translator3, "AST translator - bitfields #3") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;
    struct kefir_ast_translator_environment env;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));
    ASSERT_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_structure(&kft_mem, context.context.type_bundle, NULL, &struct_type1);
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field1",
                                             kefir_ast_type_unsigned_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 16)));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context.context.symbols, struct_type1, "field2",
                                          kefir_ast_type_char(), NULL));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field3",
                                             kefir_ast_type_unsigned_char(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 3)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field4",
                                             kefir_ast_type_unsigned_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 4)));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context.context.symbols, struct_type1, "field5",
                                          kefir_ast_type_unsigned_char(), NULL));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field6",
                                             kefir_ast_type_unsigned_short(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 1)));

    struct kefir_irbuilder_type type_builder;
    struct kefir_ast_designator designator = {.type = KEFIR_AST_DESIGNATOR_MEMBER, .member = NULL, .next = NULL};
    struct kefir_ast_type_layout *member_layout = NULL;

    struct kefir_ir_type ir_type1;
    struct kefir_ast_type_layout *layout1 = NULL;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 0, &ir_type1));
    ASSERT_OK(kefir_irbuilder_type_init(&kft_mem, &type_builder, &ir_type1));
    ASSERT_OK(kefir_ast_translate_object_type(&kft_mem, type1, 0, &env, &type_builder, &layout1));

#define ASSERT_MEMBER(_id, _pos, _width, _offset)                                                   \
    do {                                                                                            \
        designator.member = (_id);                                                                  \
        ASSERT_OK(kefir_ast_type_layout_resolve(layout1, &designator, &member_layout, NULL, NULL)); \
        ASSERT(member_layout->value == (_pos));                                                     \
        struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(&ir_type1, member_layout->value);   \
        ASSERT(typeentry->typecode == KEFIR_IR_TYPE_BITS);                                          \
        ASSERT(member_layout->bitfield);                                                            \
        ASSERT(member_layout->bitfield_props.width == (_width));                                    \
        ASSERT(member_layout->bitfield_props.offset == (_offset));                                  \
    } while (0)

    ASSERT_MEMBER("field1", 1, 16, 0);
    ASSERT_MEMBER("field3", 3, 3, 0);
    ASSERT_MEMBER("field4", 3, 4, 3);
    ASSERT_MEMBER("field6", 5, 1, 0);

#undef ASSERT_MEMBER

    ASSERT(kefir_ir_type_total_length(&ir_type1) == 6);

    ASSERT_OK(kefir_ast_type_layout_free(&kft_mem, layout1));
    ASSERT_OK(KEFIR_IRBUILDER_TYPE_FREE(&type_builder));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &ir_type1));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_bitfield_translator4, "AST translator - union bitfields #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;
    struct kefir_ast_translator_environment env;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));
    ASSERT_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_struct_type *union_type1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_union(&kft_mem, context.context.type_bundle, NULL, &union_type1);
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, union_type1, "field1",
                                             kefir_ast_type_signed_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 2)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, union_type1, "field2",
                                             kefir_ast_type_signed_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 3)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, union_type1, "field3",
                                             kefir_ast_type_signed_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 6)));
    ASSERT_OK(kefir_ast_struct_type_field(&kft_mem, context.context.symbols, union_type1, "field4",
                                          kefir_ast_type_unsigned_long_long(), NULL));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, union_type1, "field5",
                                             kefir_ast_type_char(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 1)));

    struct kefir_irbuilder_type type_builder;
    struct kefir_ast_designator designator = {.type = KEFIR_AST_DESIGNATOR_MEMBER, .member = NULL, .next = NULL};
    struct kefir_ast_type_layout *member_layout = NULL;

    struct kefir_ir_type ir_type1;
    struct kefir_ast_type_layout *layout1 = NULL;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 0, &ir_type1));
    ASSERT_OK(kefir_irbuilder_type_init(&kft_mem, &type_builder, &ir_type1));
    ASSERT_OK(kefir_ast_translate_object_type(&kft_mem, type1, 0, &env, &type_builder, &layout1));

#define ASSERT_MEMBER(_id, _pos, _width, _offset)                                                   \
    do {                                                                                            \
        designator.member = (_id);                                                                  \
        ASSERT_OK(kefir_ast_type_layout_resolve(layout1, &designator, &member_layout, NULL, NULL)); \
        ASSERT(member_layout->value == (_pos));                                                     \
        ASSERT(member_layout->bitfield);                                                            \
        ASSERT(member_layout->bitfield_props.width == (_width));                                    \
        ASSERT(member_layout->bitfield_props.offset == (_offset));                                  \
    } while (0)

    ASSERT_MEMBER("field1", 1, 2, 0);
    ASSERT_MEMBER("field2", 2, 3, 0);
    ASSERT_MEMBER("field3", 3, 6, 0);
    ASSERT_MEMBER("field5", 5, 1, 0);

#undef ASSERT_MEMBER

    ASSERT(kefir_ir_type_total_length(&ir_type1) == 6);

    ASSERT_OK(kefir_ast_type_layout_free(&kft_mem, layout1));
    ASSERT_OK(KEFIR_IRBUILDER_TYPE_FREE(&type_builder));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &ir_type1));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE

DEFINE_CASE(ast_bitfield_translator5, "AST translator - bitfields alignments #1") {
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context context;
    struct kefir_ast_translator_environment env;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                            &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &context));
    ASSERT_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_struct_type *struct_type1 = NULL;
    const struct kefir_ast_type *type1 =
        kefir_ast_type_structure(&kft_mem, context.context.type_bundle, NULL, &struct_type1);
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field1",
                                             kefir_ast_type_char(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 7)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field2",
                                             kefir_ast_type_char(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 3)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field3",
                                             kefir_ast_type_signed_short(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 2)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field4",
                                             kefir_ast_type_signed_short(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 8)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field5",
                                             kefir_ast_type_unsigned_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 6)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field6",
                                             kefir_ast_type_signed_char(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 3)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field7",
                                             kefir_ast_type_unsigned_short(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 1)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field8",
                                             kefir_ast_type_signed_long(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 1)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field9",
                                             kefir_ast_type_signed_long(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 20)));
    ASSERT_OK(kefir_ast_struct_type_bitfield(&kft_mem, context.context.symbols, struct_type1, "field10",
                                             kefir_ast_type_unsigned_int(), NULL,
                                             kefir_ast_constant_expression_integer(&kft_mem, 2)));

    struct kefir_irbuilder_type type_builder;
    struct kefir_ast_designator designator = {.type = KEFIR_AST_DESIGNATOR_MEMBER, .member = NULL, .next = NULL};
    struct kefir_ast_type_layout *member_layout = NULL;

    struct kefir_ir_type ir_type1;
    struct kefir_ast_type_layout *layout1 = NULL;
    ASSERT_OK(kefir_ir_type_alloc(&kft_mem, 0, &ir_type1));
    ASSERT_OK(kefir_irbuilder_type_init(&kft_mem, &type_builder, &ir_type1));
    ASSERT_OK(kefir_ast_translate_object_type(&kft_mem, type1, 0, &env, &type_builder, &layout1));

#define ASSERT_MEMBER(_id, _pos, _width, _offset)                                                   \
    do {                                                                                            \
        designator.member = (_id);                                                                  \
        ASSERT_OK(kefir_ast_type_layout_resolve(layout1, &designator, &member_layout, NULL, NULL)); \
        ASSERT(member_layout->value == (_pos));                                                     \
        struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(&ir_type1, member_layout->value);   \
        ASSERT(typeentry->typecode == KEFIR_IR_TYPE_BITS);                                          \
        ASSERT(member_layout->bitfield);                                                            \
        ASSERT(member_layout->bitfield_props.width == (_width));                                    \
        ASSERT(member_layout->bitfield_props.offset == (_offset));                                  \
    } while (0)

    ASSERT_MEMBER("field1", 1, 7, 0);
    ASSERT_MEMBER("field2", 2, 3, 0);
    ASSERT_MEMBER("field3", 2, 2, 3);
    ASSERT_MEMBER("field4", 3, 8, 0);
    ASSERT_MEMBER("field5", 3, 6, 8);
    ASSERT_MEMBER("field6", 4, 3, 0);
    ASSERT_MEMBER("field7", 4, 1, 3);
    ASSERT_MEMBER("field8", 4, 1, 4);
    ASSERT_MEMBER("field9", 4, 20, 5);
    ASSERT_MEMBER("field10", 4, 2, 25);

#undef ASSERT_MEMBER

    ASSERT(kefir_ir_type_total_length(&ir_type1) == 5);

    ASSERT_OK(kefir_ast_type_layout_free(&kft_mem, layout1));
    ASSERT_OK(KEFIR_IRBUILDER_TYPE_FREE(&type_builder));
    ASSERT_OK(kefir_ir_type_free(&kft_mem, &ir_type1));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
}
END_CASE
