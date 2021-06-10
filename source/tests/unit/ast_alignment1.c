#include "kefir/test/unit_test.h"
#include "kefir/ast/alignment.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/local_context.h"
#include "kefir/test/util.h"

DEFINE_CASE(ast_alignment_default, "AST alignment - default")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

struct kefir_ast_alignment *alignment = kefir_ast_alignment_default(&kft_mem);
ASSERT(alignment != NULL);
ASSERT_OK(kefir_ast_alignment_evaluate(&kft_mem, context, alignment));
ASSERT(alignment->value == KEFIR_AST_DEFAULT_ALIGNMENT);
ASSERT_OK(kefir_ast_alignment_free(&kft_mem, alignment));

ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

#define ASSERT_ALIGN_AS_TYPE(_mem, _context, _type, _value)                                   \
    do {                                                                                      \
        struct kefir_ast_alignment *alignment = kefir_ast_alignment_as_type((_mem), (_type)); \
        ASSERT(alignment != NULL);                                                            \
        ASSERT_OK(kefir_ast_analyze_alignment((_mem), (_context), alignment));                \
        ASSERT_OK(kefir_ast_alignment_evaluate((_mem), (_context), alignment));               \
        ASSERT(alignment->value == (_value));                                                 \
        ASSERT_OK(kefir_ast_alignment_free((_mem), alignment));                               \
    } while (0)

DEFINE_CASE(ast_alignment_as_type, "AST alignment - as type")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

ASSERT_ALIGN_AS_TYPE(&kft_mem, context, kefir_ast_type_bool(), 1);
ASSERT_ALIGN_AS_TYPE(&kft_mem, context, kefir_ast_type_char(), 1);
ASSERT_ALIGN_AS_TYPE(&kft_mem, context, kefir_ast_type_unsigned_char(), 1);
ASSERT_ALIGN_AS_TYPE(&kft_mem, context, kefir_ast_type_signed_char(), 1);

ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

#undef ASSERT_ALIGN_AS_TYPE

#define ASSERT_ALIGN_AS_CONST_EXPR(_mem, _context, _const_expr, _value)                                      \
    do {                                                                                                     \
        struct kefir_ast_alignment *alignment = kefir_ast_alignment_const_expression((_mem), (_const_expr)); \
        ASSERT(alignment != NULL);                                                                           \
        ASSERT_OK(kefir_ast_analyze_alignment((_mem), (_context), alignment));                               \
        ASSERT_OK(kefir_ast_alignment_evaluate((_mem), (_context), alignment));                              \
        ASSERT(alignment->value == (_value));                                                                \
        ASSERT_OK(kefir_ast_alignment_free((_mem), alignment));                                              \
    } while (0)

DEFINE_CASE(ast_alignment_as_const_expr, "AST alignment - as constant expression")
const struct kefir_ast_type_traits *type_traits = kefir_ast_default_type_traits();
struct kefir_ast_global_context global_context;
struct kefir_ast_local_context local_context;

ASSERT_OK(kefir_ast_global_context_init(&kft_mem, type_traits, &kft_util_get_translator_environment()->target_env,
                                        &global_context));
ASSERT_OK(kefir_ast_local_context_init(&kft_mem, &global_context, &local_context));
struct kefir_ast_context *context = &local_context.context;

for (kefir_size_t i = 0; i < 5; i++) {
    ASSERT_ALIGN_AS_CONST_EXPR(&kft_mem, context, kefir_ast_constant_expression_integer(&kft_mem, 1 << i), 1u << i);

    ASSERT_ALIGN_AS_CONST_EXPR(
        &kft_mem, context,
        kefir_ast_new_constant_expression(&kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(&kft_mem, 1 << i))),
        1u << i);

    ASSERT_ALIGN_AS_CONST_EXPR(&kft_mem, context,
                               kefir_ast_new_constant_expression(
                                   &kft_mem, KEFIR_AST_NODE_BASE(kefir_ast_new_binary_operation(
                                                 &kft_mem, KEFIR_AST_OPERATION_SHIFT_LEFT,
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(&kft_mem, 1)),
                                                 KEFIR_AST_NODE_BASE(kefir_ast_new_constant_uint(&kft_mem, i))))),
                               1u << i);
}

ASSERT_OK(kefir_ast_local_context_free(&kft_mem, &local_context));
ASSERT_OK(kefir_ast_global_context_free(&kft_mem, &global_context));
END_CASE

#undef ASSERT_ALIGN_AS_CONST_EXPR
