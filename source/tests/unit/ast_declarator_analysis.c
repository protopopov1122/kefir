#include "kefir/test/unit_test.h"
#include "kefir/ast/analyzer/declarator.h"
#include "kefir/ast/global_context.h"
#include "kefir/ast/local_context.h"
#include "kefir/test/util.h"
#include <stdarg.h>

static kefir_result_t append_specifiers(struct kefir_mem *mem,
                                      struct kefir_ast_declarator_specifier_list *list,
                                      int count,
                                      ...) {
    va_list args;
    va_start(args, count);
    while (count--) {
        struct kefir_ast_declarator_specifier *specifier = va_arg(args, struct kefir_ast_declarator_specifier *);
        REQUIRE_OK(kefir_ast_declarator_specifier_list_append(mem, list, specifier));
    }
    va_end(args);
    return KEFIR_OK;
}

#define ASSERT_IDENTIFIER_TYPE(_mem, _context, _type, _spec_count, ...) \
    do { \
        struct kefir_ast_declarator_specifier_list specifiers; \
        ASSERT_OK(kefir_ast_declarator_specifier_list_init(&specifiers)); \
        ASSERT_OK(append_specifiers((_mem), &specifiers, (_spec_count), __VA_ARGS__)); \
     \
        struct kefir_ast_declarator *declarator = kefir_ast_declarator_identifier((_mem), \
            (_context)->symbols, "var"); \
         \
        const struct kefir_ast_type *type = NULL; \
        kefir_ast_scoped_identifier_storage_t storage; \
        struct kefir_ast_function_specifier function_specifier; \
        ASSERT_OK(kefir_ast_analyze_declaration((_mem), (_context)->type_bundle, &specifiers, \
            declarator, &type, &storage, &function_specifier)); \
     \
        ASSERT(type != NULL); \
        ASSERT(KEFIR_AST_TYPE_SAME(type, (_type))); \
     \
        ASSERT_OK(kefir_ast_declarator_free((_mem), declarator)); \
        ASSERT_OK(kefir_ast_declarator_specifier_list_free((_mem), &specifiers)); \
    } while (0)

DEFINE_CASE(ast_declarator_analysis1, "AST declarator analysis - declarator type #1")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_void(), 1,
        kefir_ast_type_specifier_void(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_char(), 1,
        kefir_ast_type_specifier_char(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_signed_char(), 2,
        kefir_ast_type_specifier_signed(&kft_mem), kefir_ast_type_specifier_char(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_unsigned_char(), 2,
        kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_char(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_signed_short(), 1,
        kefir_ast_type_specifier_short(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_signed_short(), 2,
        kefir_ast_type_specifier_signed(&kft_mem), kefir_ast_type_specifier_short(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_signed_short(), 2,
        kefir_ast_type_specifier_short(&kft_mem), kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_signed_short(), 3,
        kefir_ast_type_specifier_signed(&kft_mem), kefir_ast_type_specifier_short(&kft_mem),
        kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_unsigned_short(), 2,
        kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_short(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_unsigned_short(), 3,
        kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_short(&kft_mem),
        kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_signed_int(), 1,
        kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_signed_int(), 1,
        kefir_ast_type_specifier_signed(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_signed_int(), 2,
        kefir_ast_type_specifier_signed(&kft_mem), kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_unsigned_int(), 1,
        kefir_ast_type_specifier_unsigned(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_unsigned_int(), 2,
        kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_signed_int(), 1,
        kefir_ast_type_specifier_signed(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_signed_long(), 1,
        kefir_ast_type_specifier_long(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_signed_long(), 2,
        kefir_ast_type_specifier_signed(&kft_mem), kefir_ast_type_specifier_long(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_signed_long(), 2,
        kefir_ast_type_specifier_long(&kft_mem), kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_signed_long(), 3,
        kefir_ast_type_specifier_signed(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
        kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_unsigned_long(), 2,
        kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_long(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_unsigned_long(), 3,
        kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
        kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_signed_long_long(), 2,
        kefir_ast_type_specifier_long(&kft_mem), kefir_ast_type_specifier_long(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_signed_long_long(), 3,
        kefir_ast_type_specifier_signed(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
        kefir_ast_type_specifier_long(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_signed_long_long(), 3,
        kefir_ast_type_specifier_long(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
        kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_signed_long_long(), 4,
        kefir_ast_type_specifier_signed(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
        kefir_ast_type_specifier_long(&kft_mem), kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_unsigned_long_long(), 3,
        kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
        kefir_ast_type_specifier_long(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_unsigned_long_long(), 4,
        kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
        kefir_ast_type_specifier_long(&kft_mem), kefir_ast_type_specifier_int(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_float(), 1,
        kefir_ast_type_specifier_float(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_double(), 1,
        kefir_ast_type_specifier_double(&kft_mem));
    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_bool(), 1,
        kefir_ast_type_specifier_bool(&kft_mem));

    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

DEFINE_CASE(ast_declarator_analysis2, "AST declarator analysis - declarator type #2")
    const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
    struct kefir_ast_global_context global_context;
    struct kefir_ast_local_context local_context;

    ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits,
        &kft_util_get_translator_environment()->target_env, &global_context));
    ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
    struct kefir_ast_context *context = &local_context.context;

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
            (struct kefir_ast_type_qualification){
                .constant = true
            }), 2,
        kefir_ast_type_qualifier_const(&kft_mem), kefir_ast_type_specifier_int(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_long_long(),
            (struct kefir_ast_type_qualification){
                .restricted = true
            }), 3,
        kefir_ast_type_specifier_long(&kft_mem), kefir_ast_type_qualifier_restrict(&kft_mem),
        kefir_ast_type_specifier_long(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_float(),
            (struct kefir_ast_type_qualification){
                .volatile_type = true
            }), 2,
        kefir_ast_type_specifier_float(&kft_mem), kefir_ast_type_qualifier_volatile(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_float(),
            (struct kefir_ast_type_qualification){
                .volatile_type = true
            }), 2,
        kefir_ast_type_specifier_float(&kft_mem), kefir_ast_type_qualifier_volatile(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_short(),
            (struct kefir_ast_type_qualification){
                .constant = true,
                .restricted = true
            }), 5,
        kefir_ast_type_specifier_unsigned(&kft_mem), kefir_ast_type_qualifier_restrict(&kft_mem),
        kefir_ast_type_specifier_int(&kft_mem), kefir_ast_type_qualifier_const(&kft_mem),
        kefir_ast_type_specifier_short(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_char(),
            (struct kefir_ast_type_qualification){
                .constant = true,
                .volatile_type = true
            }), 4,
        kefir_ast_type_specifier_char(&kft_mem), kefir_ast_type_specifier_signed(&kft_mem),
        kefir_ast_type_qualifier_volatile(&kft_mem), kefir_ast_type_qualifier_const(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_signed_int(),
            (struct kefir_ast_type_qualification){
                .restricted = true,
                .volatile_type = true
            }), 2,
        kefir_ast_type_qualifier_volatile(&kft_mem), kefir_ast_type_qualifier_restrict(&kft_mem));

    ASSERT_IDENTIFIER_TYPE(&kft_mem, context,
        kefir_ast_type_qualified(&kft_mem, context->type_bundle, kefir_ast_type_unsigned_long(),
            (struct kefir_ast_type_qualification){
                .constant = true,
                .restricted = true,
                .volatile_type = true
            }), 5,
        kefir_ast_type_qualifier_restrict(&kft_mem), kefir_ast_type_specifier_long(&kft_mem),
        kefir_ast_type_qualifier_const(&kft_mem), kefir_ast_type_specifier_unsigned(&kft_mem),
        kefir_ast_type_qualifier_volatile(&kft_mem));
    
    ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
    ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE
