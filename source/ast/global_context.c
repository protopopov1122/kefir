#include <string.h>
#include "kefir/ast/global_context.h"
#include "kefir/ast/runtime.h"
#include "kefir/ast/context_impl.h"
#include "kefir/ast/analyzer/initializer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t context_resolve_ordinary_identifier(const struct kefir_ast_context *context,
                                                          const char *identifier,
                                                          const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    ASSIGN_DECL_CAST(struct kefir_ast_global_context *, global_ctx, context->payload);
    return kefir_ast_global_context_resolve_scoped_ordinary_identifier(global_ctx, identifier, scoped_id);
}

static kefir_result_t context_resolve_tag_identifier(const struct kefir_ast_context *context, const char *identifier,
                                                     const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    ASSIGN_DECL_CAST(struct kefir_ast_global_context *, global_ctx, context->payload);
    return kefir_ast_global_context_resolve_scoped_tag_identifier(global_ctx, identifier, scoped_id);
}

static kefir_result_t context_allocate_temporary_value(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                       const struct kefir_ast_type *type,
                                                       struct kefir_ast_temporary_identifier *temp_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE(temp_id != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid temporary identifier pointer"));

    ASSIGN_DECL_CAST(struct kefir_ast_global_context *, global_ctx, context->payload);
    if (kefir_ast_temporaries_init(mem, context->type_bundle, context->temporaries)) {
        REQUIRE(context->temporaries->type != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to initialize a temporary type"));
        REQUIRE_OK(kefir_ast_global_context_define_static(mem, global_ctx, KEFIR_AST_TRANSLATOR_TEMPORARIES_IDENTIFIER,
                                                          context->temporaries->type, NULL, NULL, NULL));
    }
    REQUIRE_OK(kefir_ast_temporaries_new_temporary(mem, context, type, temp_id));
    return KEFIR_OK;
}

static kefir_result_t context_define_tag(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                         const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    ASSIGN_DECL_CAST(struct kefir_ast_global_context *, global_ctx, context->payload);
    REQUIRE_OK(kefir_ast_global_context_define_tag(mem, global_ctx, type, NULL));
    return KEFIR_OK;
}

static kefir_result_t context_define_constant(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                              const char *identifier, struct kefir_ast_constant_expression *value,
                                              const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant expression"));

    ASSIGN_DECL_CAST(struct kefir_ast_global_context *, global_ctx, context->payload);
    REQUIRE_OK(kefir_ast_global_context_define_constant(mem, global_ctx, identifier, value, type, NULL));
    return KEFIR_OK;
}

static kefir_result_t context_define_identifier(
    struct kefir_mem *mem, const struct kefir_ast_context *context, kefir_bool_t declaration, const char *identifier,
    const struct kefir_ast_type *type, kefir_ast_scoped_identifier_storage_t storage_class,
    kefir_ast_function_specifier_t function_specifier, struct kefir_ast_alignment *alignment,
    struct kefir_ast_initializer *initializer, const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(identifier != NULL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Empty identifiers are not permitted in the global scope"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    ASSIGN_DECL_CAST(struct kefir_ast_global_context *, global_ctx, context->payload);

    kefir_bool_t is_function = type->tag == KEFIR_AST_TYPE_FUNCTION;
    if (is_function) {
        REQUIRE(
            strcmp(identifier, type->function_type.identifier) == 0,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Provided identifier does not much identifier from function type"));
        REQUIRE(alignment == NULL,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "Alignment specifier is not permitted in the declaration of function"));
        REQUIRE(initializer == NULL,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function cannot be defined with initializer"));
        switch (storage_class) {
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_TYPEDEF:
                REQUIRE(function_specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                        "Functin specifiers may only be used in function declarations"));
                REQUIRE(declaration, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                                     "Typedef specifier cannot be used for function definition"));
                REQUIRE_OK(kefir_ast_global_context_define_type(mem, global_ctx, identifier, type, scoped_id));
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN:
                if (declaration) {
                    REQUIRE_OK(kefir_ast_global_context_declare_function(mem, global_ctx, function_specifier, type,
                                                                         scoped_id));
                } else {
                    REQUIRE_OK(
                        kefir_ast_global_context_define_function(mem, global_ctx, function_specifier, type, scoped_id));
                }
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC:
                REQUIRE_OK(kefir_ast_global_context_define_static_function(mem, global_ctx, function_specifier, type,
                                                                           scoped_id));
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN:
                REQUIRE_OK(
                    kefir_ast_global_context_define_function(mem, global_ctx, function_specifier, type, scoped_id));
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER:
                return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Illegal function storage-class specifier");
        }
    } else {
        REQUIRE(function_specifier == KEFIR_AST_FUNCTION_SPECIFIER_NONE,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function specifiers cannot be used for non-function types"));
        REQUIRE(declaration || initializer != NULL,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Initializer must be provided to for identifier definition"));
        switch (storage_class) {
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_TYPEDEF:
                REQUIRE(alignment == NULL,
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Alignment cannot be specifier to type definitions"));
                REQUIRE_OK(kefir_ast_global_context_define_type(mem, global_ctx, identifier, type, scoped_id));
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN:
                if (declaration) {
                    REQUIRE_OK(kefir_ast_global_context_declare_external(mem, global_ctx, identifier, type, alignment,
                                                                         scoped_id));
                } else {
                    REQUIRE_OK(kefir_ast_global_context_define_external(mem, global_ctx, identifier, type, alignment,
                                                                        initializer, scoped_id));
                }
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC:
                REQUIRE_OK(kefir_ast_global_context_define_static(mem, global_ctx, identifier, type, alignment,
                                                                  initializer, scoped_id));
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL:
                REQUIRE_OK(kefir_ast_global_context_define_external_thread_local(mem, global_ctx, identifier, type,
                                                                                 alignment, initializer, scoped_id));
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL:
                if (declaration) {
                    REQUIRE_OK(kefir_ast_global_context_declare_external_thread_local(mem, global_ctx, identifier, type,
                                                                                      alignment, scoped_id));
                } else {
                    REQUIRE_OK(kefir_ast_global_context_define_external_thread_local(
                        mem, global_ctx, identifier, type, alignment, initializer, scoped_id));
                }
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL:
                REQUIRE_OK(kefir_ast_global_context_define_static_thread_local(mem, global_ctx, identifier, type,
                                                                               alignment, initializer, scoped_id));
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN:
                REQUIRE_OK(kefir_ast_global_context_define_external(mem, global_ctx, identifier, type, alignment,
                                                                    initializer, scoped_id));
                break;

            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO:
            case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER:
                return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Illegal file-scope identifier storage class");
        }
    }

    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_init(struct kefir_mem *mem, const struct kefir_ast_type_traits *type_traits,
                                             const struct kefir_ast_target_environment *target_env,
                                             struct kefir_ast_global_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type_traits != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type traits"));
    REQUIRE(target_env != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST target environment"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_symbol_table_init(&context->symbols));
    REQUIRE_OK(kefir_ast_type_bundle_init(&context->type_bundle, &context->symbols));
    context->type_traits = type_traits;
    context->target_env = target_env;
    context->temporaries = (struct kefir_ast_context_temporaries){0};
    REQUIRE_OK(kefir_ast_identifier_flat_scope_init(&context->tag_scope));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_on_removal(&context->tag_scope, kefir_ast_context_free_scoped_identifier,
                                                          NULL));

    REQUIRE_OK(kefir_ast_identifier_flat_scope_init(&context->object_identifiers));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_on_removal(&context->object_identifiers,
                                                          kefir_ast_context_free_scoped_identifier, NULL));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_init(&context->constant_identifiers));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_on_removal(&context->constant_identifiers,
                                                          kefir_ast_context_free_scoped_identifier, NULL));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_init(&context->type_identifiers));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_on_removal(&context->type_identifiers,
                                                          kefir_ast_context_free_scoped_identifier, NULL));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_init(&context->function_identifiers));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_on_removal(&context->function_identifiers,
                                                          kefir_ast_context_free_scoped_identifier, NULL));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_init(&context->ordinary_scope));

    context->context.resolve_ordinary_identifier = context_resolve_ordinary_identifier;
    context->context.resolve_tag_identifier = context_resolve_tag_identifier;
    context->context.allocate_temporary_value = context_allocate_temporary_value;
    context->context.define_tag = context_define_tag;
    context->context.define_constant = context_define_constant;
    context->context.define_identifier = context_define_identifier;
    context->context.symbols = &context->symbols;
    context->context.type_bundle = &context->type_bundle;
    context->context.type_traits = context->type_traits;
    context->context.target_env = context->target_env;
    context->context.temporaries = &context->temporaries;
    context->context.type_analysis_context = KEFIR_AST_TYPE_ANALYSIS_DEFAULT;
    context->context.payload = context;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_free(struct kefir_mem *mem, struct kefir_ast_global_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_free(mem, &context->tag_scope));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_free(mem, &context->ordinary_scope));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_free(mem, &context->constant_identifiers));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_free(mem, &context->type_identifiers));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_free(mem, &context->function_identifiers));
    REQUIRE_OK(kefir_ast_identifier_flat_scope_free(mem, &context->object_identifiers));
    REQUIRE_OK(kefir_ast_type_bundle_free(mem, &context->type_bundle));
    REQUIRE_OK(kefir_symbol_table_free(mem, &context->symbols));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_resolve_scoped_ordinary_identifier(
    const struct kefir_ast_global_context *context, const char *identifier,
    const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST scoped identifier pointer"));
    struct kefir_ast_scoped_identifier *result = NULL;
    REQUIRE_OK(kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier, &result));
    *scoped_id = result;
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_resolve_scoped_tag_identifier(
    const struct kefir_ast_global_context *context, const char *identifier,
    const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST scoped identifier pointer"));
    struct kefir_ast_scoped_identifier *result = NULL;
    REQUIRE_OK(kefir_ast_identifier_flat_scope_at(&context->tag_scope, identifier, &result));
    *scoped_id = result;
    return KEFIR_OK;
}

static kefir_result_t insert_ordinary_identifier(struct kefir_mem *mem, struct kefir_ast_global_context *context,
                                                 const char *identifier,
                                                 struct kefir_ast_scoped_identifier *ordinary_id) {
    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier, &scoped_id);
    if (res == KEFIR_NOT_FOUND) {
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->ordinary_scope, identifier, ordinary_id);
    }
    return res;
}

static kefir_result_t require_ordinary_identifier_type(struct kefir_ast_global_context *context, const char *identifier,
                                                       kefir_ast_scoped_identifier_class_t klass) {
    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE(scoped_id->klass == klass,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine identifier with different kind of symbol"));
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_declare_external(struct kefir_mem *mem,
                                                         struct kefir_ast_global_context *context,
                                                         const char *identifier, const struct kefir_ast_type *type,
                                                         struct kefir_ast_alignment *alignment,
                                                         const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    REQUIRE_OK(require_ordinary_identifier_type(context, identifier, KEFIR_AST_SCOPE_IDENTIFIER_OBJECT));

    identifier = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));

    struct kefir_ast_scoped_identifier *ordinary_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->object_identifiers, identifier, &ordinary_id);
    if (res == KEFIR_OK) {
        REQUIRE(
            ordinary_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN ||
                ordinary_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, ordinary_id->object.type, type),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "All declarations of the same identifier shall have compatible types"));
        REQUIRE_OK(kefir_ast_context_merge_alignment(mem, &ordinary_id->object.alignment, alignment));
        ordinary_id->object.type =
            KEFIR_AST_TYPE_COMPOSITE(mem, &context->type_bundle, context->type_traits, ordinary_id->object.type, type);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        ordinary_id = kefir_ast_context_allocate_scoped_object_identifier(
            mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, alignment,
            KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE, true, NULL);
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->object_identifiers, identifier, ordinary_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, ordinary_id, NULL);
            return res;
        });
    }

    REQUIRE_OK(insert_ordinary_identifier(mem, context, identifier, ordinary_id));
    ASSIGN_PTR(scoped_id, ordinary_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_declare_external_thread_local(
    struct kefir_mem *mem, struct kefir_ast_global_context *context, const char *identifier,
    const struct kefir_ast_type *type, struct kefir_ast_alignment *alignment,
    const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    REQUIRE_OK(require_ordinary_identifier_type(context, identifier, KEFIR_AST_SCOPE_IDENTIFIER_OBJECT));

    identifier = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));

    struct kefir_ast_scoped_identifier *ordinary_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->object_identifiers, identifier, &ordinary_id);
    if (res == KEFIR_OK) {
        REQUIRE(
            ordinary_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL ||
                ordinary_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, ordinary_id->object.type, type),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "All declarations of the same identifier shall have compatible types"));
        REQUIRE_OK(kefir_ast_context_merge_alignment(mem, &ordinary_id->object.alignment, alignment));
        ordinary_id->object.type =
            KEFIR_AST_TYPE_COMPOSITE(mem, &context->type_bundle, context->type_traits, ordinary_id->object.type, type);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        ordinary_id = kefir_ast_context_allocate_scoped_object_identifier(
            mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL, alignment,
            KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE, true, NULL);
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->object_identifiers, identifier, ordinary_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, ordinary_id, NULL);
            return res;
        });
    }

    REQUIRE_OK(insert_ordinary_identifier(mem, context, identifier, ordinary_id));
    ASSIGN_PTR(scoped_id, ordinary_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_external(struct kefir_mem *mem, struct kefir_ast_global_context *context,
                                                        const char *identifier, const struct kefir_ast_type *type,
                                                        struct kefir_ast_alignment *alignment,
                                                        struct kefir_ast_initializer *initializer,
                                                        const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    REQUIRE_OK(require_ordinary_identifier_type(context, identifier, KEFIR_AST_SCOPE_IDENTIFIER_OBJECT));

    identifier = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));

    if (initializer != NULL) {
        struct kefir_ast_type_qualification qualifications;
        REQUIRE_OK(kefir_ast_type_retrieve_qualifications(&qualifications, type));
        struct kefir_ast_initializer_properties props;
        REQUIRE_OK(kefir_ast_analyze_initializer(mem, &context->context, type, initializer, &props));
        type = props.type;
        REQUIRE(props.constant,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "Initializers of object with static storage duration shall be constant"));

        if (!KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&qualifications)) {
            type = kefir_ast_type_qualified(mem, &context->type_bundle, type, qualifications);
        }
    }

    struct kefir_ast_scoped_identifier *ordinary_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->object_identifiers, identifier, &ordinary_id);
    if (res == KEFIR_OK) {
        REQUIRE(
            ordinary_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, ordinary_id->object.type, type),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "All declarations of the same identifier shall have compatible types"));
        REQUIRE(initializer == NULL || ordinary_id->object.initializer == NULL,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Redefinition of identifier is not permitted"));
        REQUIRE_OK(kefir_ast_context_merge_alignment(mem, &ordinary_id->object.alignment, alignment));
        ordinary_id->object.type =
            KEFIR_AST_TYPE_COMPOSITE(mem, &context->type_bundle, context->type_traits, ordinary_id->object.type, type);
        ordinary_id->object.external = false;
        if (initializer != NULL) {
            ordinary_id->object.initializer = initializer;
        }
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        ordinary_id = kefir_ast_context_allocate_scoped_object_identifier(
            mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, alignment,
            KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE, false, initializer);
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->object_identifiers, identifier, ordinary_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, ordinary_id, NULL);
            return res;
        });
    }

    REQUIRE_OK(insert_ordinary_identifier(mem, context, identifier, ordinary_id));
    ASSIGN_PTR(scoped_id, ordinary_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_external_thread_local(
    struct kefir_mem *mem, struct kefir_ast_global_context *context, const char *identifier,
    const struct kefir_ast_type *type, struct kefir_ast_alignment *alignment, struct kefir_ast_initializer *initializer,
    const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    REQUIRE_OK(require_ordinary_identifier_type(context, identifier, KEFIR_AST_SCOPE_IDENTIFIER_OBJECT));

    identifier = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));

    if (initializer != NULL) {
        struct kefir_ast_type_qualification qualifications;
        REQUIRE_OK(kefir_ast_type_retrieve_qualifications(&qualifications, type));
        struct kefir_ast_initializer_properties props;
        REQUIRE_OK(kefir_ast_analyze_initializer(mem, &context->context, type, initializer, &props));
        type = props.type;
        REQUIRE(props.constant,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "Initializers of object with thread local storage duration shall be constant"));

        if (!KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&qualifications)) {
            type = kefir_ast_type_qualified(mem, &context->type_bundle, type, qualifications);
        }
    }

    struct kefir_ast_scoped_identifier *ordinary_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->object_identifiers, identifier, &ordinary_id);
    if (res == KEFIR_OK) {
        REQUIRE(
            ordinary_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, ordinary_id->object.type, type),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "All declarations of the same identifier shall have compatible types"));
        REQUIRE(initializer == NULL || ordinary_id->object.initializer == NULL,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Redefinition of identifier is not permitted"));
        REQUIRE_OK(kefir_ast_context_merge_alignment(mem, &ordinary_id->object.alignment, alignment));
        ordinary_id->object.type =
            KEFIR_AST_TYPE_COMPOSITE(mem, &context->type_bundle, context->type_traits, ordinary_id->object.type, type);
        ordinary_id->object.external = false;
        if (initializer != NULL) {
            ordinary_id->object.initializer = initializer;
        }
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        ordinary_id = kefir_ast_context_allocate_scoped_object_identifier(
            mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL, alignment,
            KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE, false, initializer);
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->object_identifiers, identifier, ordinary_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, ordinary_id, NULL);
            return res;
        });
    }

    REQUIRE_OK(insert_ordinary_identifier(mem, context, identifier, ordinary_id));
    ASSIGN_PTR(scoped_id, ordinary_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_static(struct kefir_mem *mem, struct kefir_ast_global_context *context,
                                                      const char *identifier, const struct kefir_ast_type *type,
                                                      struct kefir_ast_alignment *alignment,
                                                      struct kefir_ast_initializer *initializer,
                                                      const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    REQUIRE_OK(require_ordinary_identifier_type(context, identifier, KEFIR_AST_SCOPE_IDENTIFIER_OBJECT));

    identifier = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));

    if (initializer != NULL) {
        struct kefir_ast_type_qualification qualifications;
        REQUIRE_OK(kefir_ast_type_retrieve_qualifications(&qualifications, type));
        struct kefir_ast_initializer_properties props;
        REQUIRE_OK(kefir_ast_analyze_initializer(mem, &context->context, type, initializer, &props));
        type = props.type;
        REQUIRE(props.constant,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "Initializers of object with static storage duration shall be constant"));

        if (!KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&qualifications)) {
            type = kefir_ast_type_qualified(mem, &context->type_bundle, type, qualifications);
        }
    }

    struct kefir_ast_scoped_identifier *ordinary_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->object_identifiers, identifier, &ordinary_id);
    if (res == KEFIR_OK) {
        REQUIRE(
            ordinary_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, ordinary_id->object.type, type),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "All declarations of the same identifier shall have compatible types"));
        REQUIRE(!ordinary_id->object.external,
                KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Identifier with static storage duration cannot be external"));
        REQUIRE(initializer == NULL || ordinary_id->object.initializer == NULL,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Redefinition of identifier is not permitted"));
        REQUIRE_OK(kefir_ast_context_merge_alignment(mem, &ordinary_id->object.alignment, alignment));
        ordinary_id->object.type =
            KEFIR_AST_TYPE_COMPOSITE(mem, &context->type_bundle, context->type_traits, ordinary_id->object.type, type);
        if (initializer != NULL) {
            ordinary_id->object.initializer = initializer;
        }
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        if (initializer == NULL) {
            REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                    "Tentative definition with internal linkage shall have complete type"));
        }
        ordinary_id = kefir_ast_context_allocate_scoped_object_identifier(
            mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC, alignment,
            KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE, false, initializer);
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->object_identifiers, identifier, ordinary_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, ordinary_id, NULL);
            return res;
        });
    }

    REQUIRE_OK(insert_ordinary_identifier(mem, context, identifier, ordinary_id));
    ASSIGN_PTR(scoped_id, ordinary_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_static_thread_local(
    struct kefir_mem *mem, struct kefir_ast_global_context *context, const char *identifier,
    const struct kefir_ast_type *type, struct kefir_ast_alignment *alignment, struct kefir_ast_initializer *initializer,
    const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    REQUIRE_OK(require_ordinary_identifier_type(context, identifier, KEFIR_AST_SCOPE_IDENTIFIER_OBJECT));

    identifier = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));

    if (initializer != NULL) {
        struct kefir_ast_type_qualification qualifications;
        REQUIRE_OK(kefir_ast_type_retrieve_qualifications(&qualifications, type));
        struct kefir_ast_initializer_properties props;
        REQUIRE_OK(kefir_ast_analyze_initializer(mem, &context->context, type, initializer, &props));
        type = props.type;
        REQUIRE(props.constant,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "Initializers of object with thread local storage duration shall be constant"));

        if (!KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&qualifications)) {
            type = kefir_ast_type_qualified(mem, &context->type_bundle, type, qualifications);
        }
    }

    struct kefir_ast_scoped_identifier *ordinary_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->object_identifiers, identifier, &ordinary_id);
    if (res == KEFIR_OK) {
        REQUIRE(
            ordinary_id->object.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, ordinary_id->object.type, type),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "All declarations of the same identifier shall have compatible types"));
        REQUIRE(!ordinary_id->object.external,
                KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Identifier with static storage duration cannot be external"));
        REQUIRE(initializer == NULL || ordinary_id->object.initializer == NULL,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Redefinition of identifier is not permitted"));
        REQUIRE_OK(kefir_ast_context_merge_alignment(mem, &ordinary_id->object.alignment, alignment));
        ordinary_id->object.type =
            KEFIR_AST_TYPE_COMPOSITE(mem, &context->type_bundle, context->type_traits, ordinary_id->object.type, type);
        if (initializer != NULL) {
            ordinary_id->object.initializer = initializer;
        }
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        if (initializer == NULL) {
            REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                    "Tentative definition with internal linkage shall have complete type"));
        }
        ordinary_id = kefir_ast_context_allocate_scoped_object_identifier(
            mem, type, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL, alignment,
            KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE, false, initializer);
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->object_identifiers, identifier, ordinary_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, ordinary_id, NULL);
            return res;
        });
    }

    REQUIRE_OK(insert_ordinary_identifier(mem, context, identifier, ordinary_id));
    ASSIGN_PTR(scoped_id, ordinary_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_constant(struct kefir_mem *mem, struct kefir_ast_global_context *context,
                                                        const char *identifier,
                                                        struct kefir_ast_constant_expression *value,
                                                        const struct kefir_ast_type *type,
                                                        const struct kefir_ast_scoped_identifier **scoped_id_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(value != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST constant expression"));

    identifier = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->ordinary_scope, identifier, &scoped_id);
    if (res == KEFIR_OK) {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine constant");
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        scoped_id = kefir_ast_context_allocate_scoped_constant(mem, value, type);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->constant_identifiers, identifier, scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, scoped_id, NULL);
            return res;
        });
        REQUIRE_OK(kefir_ast_identifier_flat_scope_insert(mem, &context->ordinary_scope, identifier, scoped_id));
    }
    ASSIGN_PTR(scoped_id_ptr, scoped_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_tag(struct kefir_mem *mem, struct kefir_ast_global_context *context,
                                                   const struct kefir_ast_type *type,
                                                   const struct kefir_ast_scoped_identifier **scoped_id_ptr) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    const char *identifier = NULL;
    REQUIRE_OK(kefir_ast_context_type_retrieve_tag(type, &identifier));

    struct kefir_ast_scoped_identifier *scoped_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->tag_scope, identifier,
                                                            (struct kefir_ast_scoped_identifier **) &scoped_id);
    if (res == KEFIR_OK) {
        REQUIRE_OK(kefir_ast_context_update_existing_scoped_type_tag(scoped_id, type));
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        scoped_id = kefir_ast_context_allocate_scoped_type_tag(mem, type);
        REQUIRE(scoped_id != NULL, KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->tag_scope, id, scoped_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, scoped_id, NULL);
            return res;
        });
    }
    ASSIGN_PTR(scoped_id_ptr, scoped_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_type(struct kefir_mem *mem, struct kefir_ast_global_context *context,
                                                    const char *identifier, const struct kefir_ast_type *type,
                                                    const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid identifier"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    REQUIRE_OK(require_ordinary_identifier_type(context, identifier, KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION));

    identifier = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
    REQUIRE(identifier != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));

    struct kefir_ast_scoped_identifier *ordinary_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->type_identifiers, identifier, &ordinary_id);
    if (res == KEFIR_OK) {
        REQUIRE(KEFIR_AST_TYPE_SAME(type, ordinary_id->type),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to redefine different type with the same identifier"));
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        ordinary_id = kefir_ast_context_allocate_scoped_type_definition(mem, type);
        REQUIRE(ordinary_id != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->type_identifiers, identifier, ordinary_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, ordinary_id, NULL);
            return res;
        });
    }

    REQUIRE_OK(insert_ordinary_identifier(mem, context, identifier, ordinary_id));
    ASSIGN_PTR(scoped_id, ordinary_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_declare_function(struct kefir_mem *mem,
                                                         struct kefir_ast_global_context *context,
                                                         kefir_ast_function_specifier_t specifier,
                                                         const struct kefir_ast_type *function,
                                                         const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(function != NULL && function->tag == KEFIR_AST_TYPE_FUNCTION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function type"));
    REQUIRE(function->function_type.identifier != NULL && strlen(function->function_type.identifier) > 0,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected function with non-empty identifier"));

    const char *identifier = function->function_type.identifier;
    REQUIRE_OK(require_ordinary_identifier_type(context, identifier, KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION));

    struct kefir_ast_scoped_identifier *ordinary_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->function_identifiers, identifier, &ordinary_id);
    if (res == KEFIR_OK) {
        REQUIRE(
            ordinary_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN ||
                ordinary_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, ordinary_id->function.type, function),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "All declarations of the same identifier shall have compatible types"));
        ordinary_id->function.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->type_bundle, context->type_traits,
                                                              ordinary_id->function.type, function);
        ordinary_id->function.specifier =
            kefir_ast_context_merge_function_specifiers(ordinary_id->function.specifier, specifier);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        ordinary_id = kefir_ast_context_allocate_scoped_function_identifier(
            mem, function, specifier, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, true);
        REQUIRE(ordinary_id != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->function_identifiers, id, ordinary_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, ordinary_id, NULL);
            return res;
        });
    }

    REQUIRE_OK(insert_ordinary_identifier(mem, context, identifier, ordinary_id));
    ASSIGN_PTR(scoped_id, ordinary_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_function(struct kefir_mem *mem, struct kefir_ast_global_context *context,
                                                        kefir_ast_function_specifier_t specifier,
                                                        const struct kefir_ast_type *function,
                                                        const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(function != NULL && function->tag == KEFIR_AST_TYPE_FUNCTION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function type"));
    REQUIRE(function->function_type.identifier != NULL && strlen(function->function_type.identifier) > 0,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected function with non-empty identifier"));

    const char *identifier = function->function_type.identifier;
    REQUIRE_OK(require_ordinary_identifier_type(context, identifier, KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION));

    struct kefir_ast_scoped_identifier *ordinary_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->function_identifiers, identifier, &ordinary_id);
    if (res == KEFIR_OK) {
        REQUIRE(
            ordinary_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN ||
                ordinary_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, ordinary_id->function.type, function),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "All declarations of the same identifier shall have compatible types"));
        ordinary_id->function.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->type_bundle, context->type_traits,
                                                              ordinary_id->function.type, function);
        ordinary_id->function.specifier =
            kefir_ast_context_merge_function_specifiers(ordinary_id->function.specifier, specifier);
        ordinary_id->function.external = false;
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        ordinary_id = kefir_ast_context_allocate_scoped_function_identifier(
            mem, function, specifier, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN, false);
        REQUIRE(ordinary_id != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->function_identifiers, id, ordinary_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, ordinary_id, NULL);
            return res;
        });
    }

    REQUIRE_OK(insert_ordinary_identifier(mem, context, identifier, ordinary_id));
    ASSIGN_PTR(scoped_id, ordinary_id);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_global_context_define_static_function(struct kefir_mem *mem,
                                                               struct kefir_ast_global_context *context,
                                                               kefir_ast_function_specifier_t specifier,
                                                               const struct kefir_ast_type *function,
                                                               const struct kefir_ast_scoped_identifier **scoped_id) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translatation context"));
    REQUIRE(function != NULL && function->tag == KEFIR_AST_TYPE_FUNCTION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST function type"));
    REQUIRE(function->function_type.identifier != NULL && strlen(function->function_type.identifier) > 0,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected function with non-empty identifier"));

    const char *identifier = function->function_type.identifier;
    REQUIRE_OK(require_ordinary_identifier_type(context, identifier, KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION));

    struct kefir_ast_scoped_identifier *ordinary_id = NULL;
    kefir_result_t res = kefir_ast_identifier_flat_scope_at(&context->function_identifiers, identifier, &ordinary_id);
    if (res == KEFIR_OK) {
        REQUIRE(
            ordinary_id->function.storage == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot declare the same identifier with different storage class"));
        REQUIRE(KEFIR_AST_TYPE_COMPATIBLE(context->type_traits, ordinary_id->function.type, function),
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                "All declarations of the same identifier shall have compatible types"));
        REQUIRE(!ordinary_id->function.external,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function identifier with static storage cannot be external"));
        ordinary_id->function.type = KEFIR_AST_TYPE_COMPOSITE(mem, &context->type_bundle, context->type_traits,
                                                              ordinary_id->function.type, function);
        ordinary_id->function.specifier =
            kefir_ast_context_merge_function_specifiers(ordinary_id->function.specifier, specifier);
    } else {
        REQUIRE(res == KEFIR_NOT_FOUND, res);
        ordinary_id = kefir_ast_context_allocate_scoped_function_identifier(
            mem, function, specifier, KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC, false);
        REQUIRE(ordinary_id != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocted AST scoped identifier"));
        const char *id = kefir_symbol_table_insert(mem, &context->symbols, identifier, NULL);
        REQUIRE(id != NULL, KEFIR_SET_ERROR(KEFIR_UNKNOWN_ERROR, "Failed to allocate identifier"));
        res = kefir_ast_identifier_flat_scope_insert(mem, &context->function_identifiers, id, ordinary_id);
        REQUIRE_ELSE(res == KEFIR_OK, {
            kefir_ast_context_free_scoped_identifier(mem, ordinary_id, NULL);
            return res;
        });
    }

    REQUIRE_OK(insert_ordinary_identifier(mem, context, identifier, ordinary_id));
    ASSIGN_PTR(scoped_id, ordinary_id);
    return KEFIR_OK;
}
