#ifndef DECLARATOR_ANALYSIS_H_
#define DECLARATOR_ANALYSIS_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/ast/declarator_specifier.h"

kefir_result_t append_specifiers(struct kefir_mem *,
                               struct kefir_ast_declarator_specifier_list *,
                               int,
                               ...);

#define ASSERT_IDENTIFIER_TYPE(_mem, _context, _type, _storage_class, _function_spec, _alignment, _spec_count, ...) \
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
        kefir_ast_function_specifier_t function_specifier; \
        struct kefir_ast_alignment *alignment = NULL; \
        const char *identifier = NULL; \
        ASSERT_OK(kefir_ast_analyze_declaration((_mem), (_context), &specifiers, \
            declarator, &identifier, &type, &storage, &function_specifier, &alignment)); \
     \
        ASSERT(strcmp(identifier, "var") == 0); \
        ASSERT(type != NULL); \
        ASSERT(KEFIR_AST_TYPE_SAME(type, (_type))); \
        ASSERT(storage == (_storage_class)); \
        ASSERT(function_specifier == (_function_spec)); \
        if (alignment == NULL) { \
            ASSERT((_alignment) == 0); \
        } else { \
            ASSERT(alignment->value == (_alignment)); \
            ASSERT_OK(kefir_ast_alignment_free((_mem), alignment)); \
        } \
     \
        ASSERT_OK(kefir_ast_declarator_free((_mem), declarator)); \
        ASSERT_OK(kefir_ast_declarator_specifier_list_free((_mem), &specifiers)); \
    } while (0)

#define ASSERT_FUNCTION_TYPE(_mem, _context, _type, _storage_class, _function_spec, _spec_count, ...) \
    do { \
        struct kefir_ast_declarator_specifier_list specifiers; \
        ASSERT_OK(kefir_ast_declarator_specifier_list_init(&specifiers)); \
        ASSERT_OK(append_specifiers((_mem), &specifiers, (_spec_count), __VA_ARGS__)); \
     \
        struct kefir_ast_declarator *declarator = kefir_ast_declarator_function((_mem), \
            kefir_ast_declarator_identifier((_mem), (_context)->symbols, "func")); \
         \
        const struct kefir_ast_type *type = NULL; \
        kefir_ast_scoped_identifier_storage_t storage; \
        kefir_ast_function_specifier_t function_specifier; \
        ASSERT_OK(kefir_ast_analyze_declaration((_mem), (_context), &specifiers, \
            declarator, NULL, &type, &storage, &function_specifier, NULL)); \
     \
        ASSERT(type != NULL); \
        ASSERT(KEFIR_AST_TYPE_SAME(type, (_type))); \
        ASSERT(storage == (_storage_class)); \
        ASSERT(function_specifier == (_function_spec)); \
     \
        ASSERT_OK(kefir_ast_declarator_free((_mem), declarator)); \
        ASSERT_OK(kefir_ast_declarator_specifier_list_free((_mem), &specifiers)); \
    } while (0)

#endif
