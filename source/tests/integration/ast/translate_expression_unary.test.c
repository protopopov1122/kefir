#include "kefir/core/mem.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/scope/translator.h"
#include "kefir/ast/local_context.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast-translator/context.h"
#include "kefir/ast-translator/scope/local_scope_layout.h"
#include "kefir/test/util.h"
#include "kefir/ir/builder.h"
#include "kefir/ir/format.h"
#include "./expression.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_ast_translator_environment env;
    REQUIRE_OK(kefir_ast_translator_environment_init(&env, kft_util_get_ir_target_platform()));

    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;
    struct kefir_ast_local_context local_context2;
    REQUIRE_OK(kefir_ast_global_context_init(mem, kefir_ast_default_type_traits(), &env.target_env, &global_context));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context));
    REQUIRE_OK(kefir_ast_local_context_init(mem, &global_context, &local_context2));
    const struct kefir_ast_context *context = &local_context.context;

    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));

    struct kefir_ast_enum_type *enum_type = NULL;
    const struct kefir_ast_type *type1 = kefir_ast_type_enumeration(mem, context->type_bundle,
        NULL, context->type_traits->underlying_enumeration_type, &enum_type);
    REQUIRE_OK(kefir_ast_enumeration_type_constant_auto(mem, context->symbols, enum_type, "A"));
    REQUIRE_OK(kefir_ast_enumeration_type_constant_auto(mem, context->symbols, enum_type, "B"));
    REQUIRE_OK(kefir_ast_enumeration_type_constant_auto(mem, context->symbols, enum_type, "C"));

    const struct kefir_ast_type *type2 = kefir_ast_type_array(mem, context->type_bundle,
        kefir_ast_type_signed_short(), kefir_ast_constant_expression_integer(mem, 16), NULL);

    struct kefir_ast_struct_type *struct_type = NULL;
    const struct kefir_ast_type *type3 = kefir_ast_type_structure(mem, context->type_bundle, NULL, &struct_type);
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, struct_type,
        "field1", kefir_ast_type_char(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, struct_type,
        "field2", kefir_ast_type_signed_long(), NULL));

    struct kefir_ast_struct_type *union_type = NULL;
    const struct kefir_ast_type *type4 = kefir_ast_type_union(mem, context->type_bundle, NULL, &union_type);
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, union_type,
        "field1", kefir_ast_type_char(), NULL));
    REQUIRE_OK(kefir_ast_struct_type_field(mem, context->symbols, union_type,
        "field2", kefir_ast_type_signed_long(), NULL));

    REQUIRE_OK(kefir_ast_global_context_define_external(mem, &global_context,
        "X", kefir_ast_type_qualified(mem, context->type_bundle, type1,
            (struct kefir_ast_type_qualification){
                .constant = true
            }), NULL, NULL));

    REQUIRE_OK(kefir_ast_global_context_define_external(mem, &global_context,
        "Y", type2, NULL, NULL));
    REQUIRE_OK(kefir_ast_global_context_define_external(mem, &global_context,
        "Z", type3, NULL, NULL));
    REQUIRE_OK(kefir_ast_global_context_define_external(mem, &global_context,
        "W", type4, NULL, NULL));

    struct kefir_ast_translator_global_scope_layout translator_global_scope;
    struct kefir_ast_translator_local_scope_layout translator_local_scope;
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_init(mem, &module, &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_init(mem, &module, &translator_global_scope, &translator_local_scope));

    struct kefir_ast_translator_context translator_context;
    REQUIRE_OK(kefir_ast_translator_context_init(&translator_context, context, &env, &module));
    REQUIRE_OK(kefir_ast_translator_build_global_scope_layout(mem, &module, &global_context, &env,
        kefir_ast_translator_context_type_resolver(&translator_context), &translator_global_scope));
    REQUIRE_OK(kefir_ast_translator_build_local_scope_layout(mem, &local_context, &env, &module,
        kefir_ast_translator_context_type_resolver(&translator_context), &translator_local_scope));
    REQUIRE_OK(kefir_ast_translate_global_scope(mem, &module, &translator_global_scope));
    struct kefir_irbuilder_block builder;

    FUNC("plus", {
        UNARY_NODE(KEFIR_AST_OPERATION_PLUS, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -100)));
        UNARY_NODE(KEFIR_AST_OPERATION_PLUS, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 100)));
        UNARY_NODE(KEFIR_AST_OPERATION_PLUS, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 3.14f)));
        UNARY_NODE(KEFIR_AST_OPERATION_PLUS, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 3.14159)));
        UNARY_NODE(KEFIR_AST_OPERATION_PLUS, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "X")));
    });

    FUNC("minus", {
        UNARY_NODE(KEFIR_AST_OPERATION_NEGATE, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -512)));
        UNARY_NODE(KEFIR_AST_OPERATION_NEGATE, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 128)));
        UNARY_NODE(KEFIR_AST_OPERATION_NEGATE, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 2.71f)));
        UNARY_NODE(KEFIR_AST_OPERATION_NEGATE, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 2.71828)));
        UNARY_NODE(KEFIR_AST_OPERATION_NEGATE, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "X")));
    });

    FUNC("invert", {
        UNARY_NODE(KEFIR_AST_OPERATION_INVERT, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'A')));
        UNARY_NODE(KEFIR_AST_OPERATION_INVERT, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, -100500)));
        UNARY_NODE(KEFIR_AST_OPERATION_INVERT, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 45)));
        UNARY_NODE(KEFIR_AST_OPERATION_INVERT, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "X")));
    });

    FUNC("logical_negate", {
        UNARY_NODE(KEFIR_AST_OPERATION_LOGICAL_NEGATE, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -1)));
        UNARY_NODE(KEFIR_AST_OPERATION_LOGICAL_NEGATE, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 0)));
        UNARY_NODE(KEFIR_AST_OPERATION_LOGICAL_NEGATE, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 1.76f)));
        UNARY_NODE(KEFIR_AST_OPERATION_LOGICAL_NEGATE, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 1.23456)));
        UNARY_NODE(KEFIR_AST_OPERATION_LOGICAL_NEGATE, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "X")));
    });

    struct kefir_ast_enum_specifier *specifier1 = kefir_ast_enum_specifier_init(mem, context->symbols, NULL, true);
    REQUIRE_OK(kefir_ast_enum_specifier_append(mem, specifier1, context->symbols, "A", NULL));
    REQUIRE_OK(kefir_ast_enum_specifier_append(mem, specifier1, context->symbols, "B", NULL));
    REQUIRE_OK(kefir_ast_enum_specifier_append(mem, specifier1, context->symbols, "C", NULL));

    struct kefir_ast_structure_specifier *specifier2 = kefir_ast_structure_specifier_init(mem, context->symbols, NULL, true);
    struct kefir_ast_structure_declaration_entry *entry1 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry1->declaration.specifiers,
        kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(mem, entry1,
        kefir_ast_declarator_identifier(mem, context->symbols, "field1"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier2,
        entry1));
    struct kefir_ast_structure_declaration_entry *entry2 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry2->declaration.specifiers,
        kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(mem, entry2,
        kefir_ast_declarator_identifier(mem, context->symbols, "field2"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier2,
        entry2));

    struct kefir_ast_structure_specifier *specifier3 = kefir_ast_structure_specifier_init(mem, context->symbols, NULL, true);
    struct kefir_ast_structure_declaration_entry *entry3 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry3->declaration.specifiers,
        kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(mem, entry3,
        kefir_ast_declarator_identifier(mem, context->symbols, "field1"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier3,
        entry3));
    struct kefir_ast_structure_declaration_entry *entry4 = kefir_ast_structure_declaration_entry_alloc(mem);
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &entry4->declaration.specifiers,
        kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_structure_declaration_entry_append(mem, entry4,
        kefir_ast_declarator_identifier(mem, context->symbols, "field2"), NULL));
    REQUIRE_OK(kefir_ast_structure_specifier_append_entry(mem, specifier3,
        entry4));

    struct kefir_ast_type_name *TYPES[] = {
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)), 
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_pointer(mem,
            kefir_ast_declarator_identifier(mem, NULL, NULL))),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_array(mem,
            KEFIR_AST_DECLARATOR_ARRAY_BOUNDED,
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 16)),
            kefir_ast_declarator_identifier(mem, NULL, NULL))),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL)),
        kefir_ast_new_type_name(mem, kefir_ast_declarator_identifier(mem, NULL, NULL))
    };
    const kefir_size_t TYPES_LEN = sizeof(TYPES) / sizeof(TYPES[0]);

    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[0]->type_decl.specifiers,
        kefir_ast_type_specifier_bool(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[1]->type_decl.specifiers,
        kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[2]->type_decl.specifiers,
        kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[2]->type_decl.specifiers,
        kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[3]->type_decl.specifiers,
        kefir_ast_type_specifier_signed(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[3]->type_decl.specifiers,
        kefir_ast_type_specifier_char(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[4]->type_decl.specifiers,
        kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[4]->type_decl.specifiers,
        kefir_ast_type_specifier_short(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[5]->type_decl.specifiers,
        kefir_ast_type_specifier_short(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[6]->type_decl.specifiers,
        kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[6]->type_decl.specifiers,
        kefir_ast_type_specifier_int(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[7]->type_decl.specifiers,
        kefir_ast_type_specifier_int(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[8]->type_decl.specifiers,
        kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[8]->type_decl.specifiers,
        kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[9]->type_decl.specifiers,
        kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[10]->type_decl.specifiers,
        kefir_ast_type_specifier_unsigned(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[10]->type_decl.specifiers,
        kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[10]->type_decl.specifiers,
        kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[11]->type_decl.specifiers,
        kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[11]->type_decl.specifiers,
        kefir_ast_type_specifier_long(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[12]->type_decl.specifiers,
        kefir_ast_type_specifier_float(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[13]->type_decl.specifiers,
        kefir_ast_type_specifier_double(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[14]->type_decl.specifiers,
        kefir_ast_type_specifier_void(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[15]->type_decl.specifiers,
        kefir_ast_type_specifier_enum(mem, specifier1)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[16]->type_decl.specifiers,
        kefir_ast_type_specifier_short(mem)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[17]->type_decl.specifiers,
        kefir_ast_type_specifier_struct(mem, specifier2)));
    REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, &TYPES[18]->type_decl.specifiers,
        kefir_ast_type_specifier_union(mem, specifier3)));


    FUNC("sizeof1", {
        for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
            UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_CLONE(mem, KEFIR_AST_NODE_BASE(TYPES[i])));
        }
    });

    FUNC("sizeof2", {
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_char(mem, 'A')));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, -100)));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(mem, 100)));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long(mem, 100200)));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong(mem, 110200)));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_long_long(mem, 110)));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_ulong_long(mem, 5110)));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_float(mem, 5.67f)));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_double(mem, 8562.8483)));

        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
            kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_void()),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)))));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "X")));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "Y")));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "Z")));
        UNARY_NODE(KEFIR_AST_OPERATION_SIZEOF, KEFIR_AST_NODE_BASE(kefir_ast_new_identifier(mem, context->symbols, "W")));
    });

    context = &local_context2.context;
    FUNC("alignof", {
        for (kefir_size_t i = 0; i < TYPES_LEN; i++) {
            UNARY_NODE(KEFIR_AST_OPERATION_ALIGNOF, KEFIR_AST_NODE_BASE(TYPES[i]));
        }
    });

    FUNC("indirection", {
        UNARY_NODE(KEFIR_AST_OPERATION_INDIRECTION, KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
            kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_char()),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 0)))));
        UNARY_NODE(KEFIR_AST_OPERATION_INDIRECTION, KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
            kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_signed_short()),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 1)))));
        UNARY_NODE(KEFIR_AST_OPERATION_INDIRECTION, KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
            kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_float()),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 2)))));
        UNARY_NODE(KEFIR_AST_OPERATION_INDIRECTION, KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
            kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_signed_long()),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 3)))));
        UNARY_NODE(KEFIR_AST_OPERATION_INDIRECTION, KEFIR_AST_NODE_BASE(kefir_ast_new_unary_operation(mem,
            KEFIR_AST_OPERATION_INDIRECTION,
            KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
                kefir_ast_type_pointer(mem, context->type_bundle,
                    kefir_ast_type_pointer(mem, context->type_bundle, kefir_ast_type_bool())),
                KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 4)))))));

        UNARY_NODE(KEFIR_AST_OPERATION_INDIRECTION, KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
            kefir_ast_type_pointer(mem, context->type_bundle, type1),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 5)))));
        UNARY_NODE(KEFIR_AST_OPERATION_INDIRECTION, KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
            kefir_ast_type_pointer(mem, context->type_bundle, type2),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 6)))));
        UNARY_NODE(KEFIR_AST_OPERATION_INDIRECTION, KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
            kefir_ast_type_pointer(mem, context->type_bundle, type3),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 7)))));
        UNARY_NODE(KEFIR_AST_OPERATION_INDIRECTION, KEFIR_AST_NODE_BASE(kefir_ast_new_cast_operator(mem,
            kefir_ast_type_pointer(mem, context->type_bundle, type4),
            KEFIR_AST_NODE_BASE(kefir_ast_new_constant_int(mem, 8)))));
    });

    REQUIRE_OK(kefir_ir_format_module(stdout, &module));

    REQUIRE_OK(kefir_ast_translator_context_free(mem, &translator_context));
    REQUIRE_OK(kefir_ast_translator_local_scope_layout_free(mem, &translator_local_scope));
    REQUIRE_OK(kefir_ast_translator_global_scope_layout_free(mem, &translator_global_scope));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context));
    REQUIRE_OK(kefir_ast_local_context_free(mem, &local_context2));
    REQUIRE_OK(kefir_ast_global_context_free(mem, &global_context));
    return KEFIR_OK;
}
