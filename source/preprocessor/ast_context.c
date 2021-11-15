#include "kefir/preprocessor/ast_context.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/core/extensions.h"

static kefir_result_t pp_resolve_ordinary_identifier(const struct kefir_ast_context *context, const char *identifier,
                                                     const struct kefir_ast_scoped_identifier **scoped_id) {
    UNUSED(context);
    UNUSED(identifier);
    UNUSED(scoped_id);
    return KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST, "Preprocessor AST context does not implement identifier resolver");
}

static kefir_result_t pp_resolve_tag_identifier(const struct kefir_ast_context *context, const char *identifier,
                                                const struct kefir_ast_scoped_identifier **scoped_id) {
    UNUSED(context);
    UNUSED(identifier);
    UNUSED(scoped_id);
    return KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST, "Preprocessor AST context does not implement tag resolver");
}

static kefir_result_t pp_resolve_label_identifier(const struct kefir_ast_context *context, const char *identifier,
                                                  const struct kefir_ast_scoped_identifier **scoped_id) {
    UNUSED(context);
    UNUSED(identifier);
    UNUSED(scoped_id);
    return KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST, "Preprocessor AST context does not implement label resolver");
}

static kefir_result_t pp_allocate_temporary_value(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                  const struct kefir_ast_type *type,
                                                  const struct kefir_source_location *source_location,
                                                  struct kefir_ast_temporary_identifier *temp_id) {
    UNUSED(mem);
    UNUSED(context);
    UNUSED(type);
    UNUSED(source_location);
    UNUSED(temp_id);
    return KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST,
                           "Preprocessor AST context does not implement temporary value allocation");
}

static kefir_result_t pp_define_tag(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                    const struct kefir_ast_type *type,
                                    const struct kefir_source_location *source_location) {
    UNUSED(mem);
    UNUSED(context);
    UNUSED(type);
    UNUSED(source_location);
    return KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST, "Preprocessor AST context does not implement tag definition");
}

static kefir_result_t pp_define_constant(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                         const char *identifier, struct kefir_ast_constant_expression *cexpr,
                                         const struct kefir_ast_type *type,
                                         const struct kefir_source_location *source_location) {
    UNUSED(mem);
    UNUSED(context);
    UNUSED(identifier);
    UNUSED(cexpr);
    UNUSED(type);
    UNUSED(source_location);
    return KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST, "Preprocessor AST context does not implement constant defintion");
}

static kefir_result_t pp_define_identifier(
    struct kefir_mem *mem, const struct kefir_ast_context *context, kefir_bool_t definition, const char *identifier,
    const struct kefir_ast_type *type, kefir_ast_scoped_identifier_storage_t storage,
    kefir_ast_function_specifier_t function, struct kefir_ast_alignment *alignment,
    struct kefir_ast_initializer *initializer, const struct kefir_source_location *source_location,
    const struct kefir_ast_scoped_identifier **scope_id) {
    UNUSED(mem);
    UNUSED(context);
    UNUSED(definition);
    UNUSED(identifier);
    UNUSED(type);
    UNUSED(storage);
    UNUSED(function);
    UNUSED(alignment);
    UNUSED(initializer);
    UNUSED(source_location);
    UNUSED(scope_id);
    return KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST, "Preprocessor AST context does not implement identifier defintion");
}

static kefir_result_t pp_reference_label(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                         const char *identifier, struct kefir_ast_flow_control_structure *parent,
                                         const struct kefir_source_location *source_location,
                                         const struct kefir_ast_scoped_identifier **scoped_id) {
    UNUSED(mem);
    UNUSED(context);
    UNUSED(identifier);
    UNUSED(parent);
    UNUSED(source_location);
    UNUSED(scoped_id);
    return KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST, "Preprocessor AST context does not implement labels");
}

static kefir_result_t pp_push_block(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                    struct kefir_ast_context_block_descriptor *block_descr) {
    UNUSED(mem);
    UNUSED(context);
    UNUSED(block_descr);
    return KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST, "Preprocessor AST context does not implement blocks");
}

static kefir_result_t pp_pop_block(struct kefir_mem *mem, const struct kefir_ast_context *context) {
    UNUSED(mem);
    UNUSED(context);
    return KEFIR_SET_ERROR(KEFIR_INVALID_REQUEST, "Preprocessor AST context does not implement blocks");
}

kefir_result_t kefir_preprocessor_ast_context_init(struct kefir_mem *mem,
                                                   struct kefir_preprocessor_ast_context *context,
                                                   struct kefir_symbol_table *symbols,
                                                   const struct kefir_ast_type_traits *type_traits,
                                                   const struct kefir_ast_target_environment *target_env,
                                                   const struct kefir_ast_context_extensions *extensions) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL,
            KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid pointer to preprocessor AST context"));
    REQUIRE(type_traits != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid type traits"));
    REQUIRE(target_env != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST target environment"));

    context->context.resolve_ordinary_identifier = pp_resolve_ordinary_identifier;
    context->context.resolve_tag_identifier = pp_resolve_tag_identifier;
    context->context.resolve_label_identifier = pp_resolve_label_identifier;
    context->context.allocate_temporary_value = pp_allocate_temporary_value;
    context->context.define_tag = pp_define_tag;
    context->context.define_constant = pp_define_constant;
    context->context.define_identifier = pp_define_identifier;
    context->context.reference_label = pp_reference_label;
    context->context.push_block = pp_push_block;
    context->context.pop_block = pp_pop_block;

    context->context.symbols = symbols;
    context->context.type_traits = type_traits;
    REQUIRE_OK(kefir_ast_type_bundle_init(&context->type_bundle, symbols));
    context->context.type_bundle = &context->type_bundle;
    context->context.target_env = target_env;
    context->context.temporaries = NULL;
    context->context.type_analysis_context = KEFIR_AST_TYPE_ANALYSIS_DEFAULT;
    context->context.flow_control_tree = NULL;
    context->context.global_context = NULL;
    context->context.function_decl_contexts = NULL;
    context->context.surrounding_function = NULL;
    context->context.payload = NULL;

    context->context.extensions = extensions;
    context->context.extensions_payload = NULL;
    kefir_result_t res;
    KEFIR_RUN_EXTENSION0(&res, mem, &context->context, on_init);
    REQUIRE_OK(res);
    return KEFIR_OK;
}

kefir_result_t kefir_preprocessor_ast_context_free(struct kefir_mem *mem,
                                                   struct kefir_preprocessor_ast_context *context) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid preprocessor AST context"));

    kefir_result_t res;
    KEFIR_RUN_EXTENSION0(&res, mem, &context->context, on_free);
    REQUIRE_OK(res);
    context->context.extensions = NULL;
    context->context.extensions_payload = NULL;

    REQUIRE_OK(kefir_ast_type_bundle_free(mem, &context->type_bundle));
    *context = (struct kefir_preprocessor_ast_context){0};
    return KEFIR_OK;
}
