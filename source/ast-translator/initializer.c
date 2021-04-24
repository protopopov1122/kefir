#include "kefir/ast-translator/initializer.h"
#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/ast/analyzer/type_traversal.h"
#include "kefir/ast-translator/typeconv.h"
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/value.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t translate_scalar(struct kefir_mem *mem,
                                     struct kefir_ast_translator_context *context,
                                     struct kefir_irbuilder_block *builder,
                                     const struct kefir_ast_type *type,
                                     const struct kefir_ast_initializer *initializer) {
    struct kefir_ast_node_base *expr = kefir_ast_initializer_head(initializer);
    if (expr != NULL) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 0));
        REQUIRE_OK(kefir_ast_translate_expression(mem, expr, builder, context));
        REQUIRE_OK(kefir_ast_translator_store_value(mem, type, context, builder));
    } else {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Scalar initializer list cannot be empty");
    }
    return KEFIR_OK;
}

static kefir_bool_t is_char_array(const struct kefir_ast_type *type, void *payload) {
    UNUSED(payload);
    return type->tag == KEFIR_AST_TYPE_ARRAY &&
        KEFIR_AST_TYPE_IS_CHARACTER(kefir_ast_unqualified_type(type->array_type.element_type));
}

static kefir_result_t layer_address(struct kefir_mem *mem,
                                  struct kefir_ast_translator_context *context,
                                  struct kefir_irbuilder_block *builder,
                                  struct kefir_ast_designator *current_designator,
                                  const struct kefir_ast_type_traversal_layer *layer) {
    const struct kefir_ast_type *type = layer->object_type;
    for (const struct kefir_ast_type_traversal_layer *top_layer = layer;
         top_layer->parent != NULL;
         top_layer = top_layer->parent, type = top_layer->object_type) {}

    const struct kefir_ast_translator_resolved_type *cached_type = NULL;
    REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_OBJECT(mem, &context->type_resolver.resolver, context->environment, context->module,
        type, 0, &cached_type));
    REQUIRE(cached_type->klass == KEFIR_AST_TRANSLATOR_RESOLVED_OBJECT_TYPE,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected cached type to be an object"));

    struct kefir_ast_designator *designator = current_designator;

    if (designator == NULL) {
        designator = kefir_ast_type_traversal_layer_designator(mem, context->ast_context->symbols, layer);
        REQUIRE(designator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to derive a designator"));
    }

    kefir_size_t offset = 0;
    struct kefir_ast_type_layout *layout = NULL;
    kefir_result_t res = kefir_ast_type_layout_resolve_offset(cached_type->object.layout, designator, &layout, &offset);
    REQUIRE_ELSE(res == KEFIR_OK, {
        if (current_designator == NULL) {
            kefir_ast_designator_free(mem, designator);
        }
        return res;
    });

    if (current_designator == NULL) {
        REQUIRE_OK(kefir_ast_designator_free(mem, designator));
    }

    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 0));
    if (offset > 0) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_IADD1, offset));
    }
    return KEFIR_OK;
}

static kefir_result_t traverse_aggregate_union(struct kefir_mem *mem,
                                             struct kefir_ast_translator_context *context,
                                             struct kefir_irbuilder_block *builder,
                                             const struct kefir_ast_type *type,
                                             const struct kefir_ast_initializer *initializer,
                                             struct kefir_ast_type_traversal *traversal) {
    UNUSED(type);
    const struct kefir_list_entry *init_iter = kefir_list_head(&initializer->list.initializers);
    for (; init_iter != NULL; kefir_list_next(&init_iter)) {
        ASSIGN_DECL_CAST(struct kefir_ast_initializer_list_entry *, entry,
            init_iter->value);
        if (entry->designator != NULL) {
            REQUIRE_OK(kefir_ast_type_traversal_navigate(mem, traversal, entry->designator));
        } else if (kefir_ast_type_traversal_empty(traversal)) {
            continue;
        }
        
        const struct kefir_ast_type_traversal_layer *layer = NULL;

        if (entry->value->type == KEFIR_AST_INITIALIZER_LIST) {
            const struct kefir_ast_type *type = NULL;
            REQUIRE_OK(kefir_ast_type_traversal_next(mem, traversal, &type, &layer));
            REQUIRE_OK(layer_address(mem, context, builder, entry->designator, layer));
            REQUIRE_OK(kefir_ast_translate_initializer(mem, context, builder, type, entry->value));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_POP, 0));
        } else if (entry->value->expression->properties.expression_props.string_literal != NULL) {
            const struct kefir_ast_type *type = NULL;
            REQUIRE_OK(kefir_ast_type_traversal_next_recursive2(mem, traversal, is_char_array, NULL, &type, &layer));
            REQUIRE_OK(layer_address(mem, context, builder, entry->designator, layer));
            REQUIRE_OK(kefir_ast_translate_expression(mem, entry->value->expression, builder, context));
            REQUIRE_OK(kefir_ast_translator_store_value(mem, type, context, builder));
        } else if (KEFIR_AST_TYPE_IS_SCALAR_TYPE(entry->value->expression->properties.type)) {
            const struct kefir_ast_type *type = NULL;
            REQUIRE_OK(kefir_ast_type_traversal_next_recursive(mem, traversal, &type, &layer));
            REQUIRE_OK(layer_address(mem, context, builder, entry->designator, layer));
            REQUIRE_OK(kefir_ast_translate_expression(mem, entry->value->expression, builder, context));
            REQUIRE_OK(kefir_ast_translate_typeconv(builder, entry->value->expression->properties.type, type));
            REQUIRE_OK(kefir_ast_translator_store_value(mem, type, context, builder));
        } else {
            const struct kefir_ast_type *type = NULL;
            REQUIRE_OK(kefir_ast_type_traversal_next(mem, traversal, &type, &layer));
            kefir_result_t res = kefir_ast_node_assignable(mem, context->ast_context,
                entry->value->expression, kefir_ast_unqualified_type(type));
            while (res == KEFIR_NO_MATCH) {
                REQUIRE_OK(kefir_ast_type_traversal_step(mem, traversal));
                REQUIRE_OK(kefir_ast_type_traversal_next(mem, traversal, &type, &layer));
                res = kefir_ast_node_assignable(mem, context->ast_context,
                    entry->value->expression, kefir_ast_unqualified_type(type));
            }
            REQUIRE_OK(res);
            REQUIRE_OK(layer_address(mem, context, builder, entry->designator, layer));
            REQUIRE_OK(kefir_ast_translate_expression(mem, entry->value->expression, builder, context));
            REQUIRE_OK(kefir_ast_translator_store_value(mem, type, context, builder));
        }
    }
    return KEFIR_OK;
}

static kefir_result_t zero_type(struct kefir_mem *mem,
                              struct kefir_ast_translator_context *context,
                              struct kefir_irbuilder_block *builder,
                              const struct kefir_ast_type *type) {
    const struct kefir_ast_translator_resolved_type *cached_type = NULL;
    REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_OBJECT(mem, &context->type_resolver.resolver, context->environment, context->module,
        type, 0, &cached_type));
    REQUIRE(cached_type->klass == KEFIR_AST_TRANSLATOR_RESOLVED_OBJECT_TYPE,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected cached type to be an object"));

    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 0));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_BZERO, cached_type->object.ir_type_id, cached_type->object.layout->value));
    return KEFIR_OK;
}

static kefir_result_t translate_struct_union(struct kefir_mem *mem,
                                           struct kefir_ast_translator_context *context,
                                           struct kefir_irbuilder_block *builder,
                                           const struct kefir_ast_type *type,
                                           const struct kefir_ast_initializer *initializer) {
    REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(type),
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot initialize incomplete object type"));
    if (initializer->type == KEFIR_AST_INITIALIZER_EXPRESSION) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 0));
        REQUIRE_OK(kefir_ast_translate_expression(mem, initializer->expression, builder, context));
        REQUIRE_OK(kefir_ast_translator_store_value(mem, type, context, builder));
    } else {
        REQUIRE_OK(zero_type(mem, context, builder, type));
        struct kefir_ast_type_traversal traversal;
        REQUIRE_OK(kefir_ast_type_traversal_init(mem, &traversal, type));
        kefir_result_t res = traverse_aggregate_union(mem, context, builder, type, initializer, &traversal);
        REQUIRE_OK(kefir_ast_type_traversal_free(mem, &traversal));
        REQUIRE_OK(res);
    }
    return KEFIR_OK;
}

static kefir_result_t translate_array(struct kefir_mem *mem,
                                    struct kefir_ast_translator_context *context,
                                    struct kefir_irbuilder_block *builder,
                                    const struct kefir_ast_type *type,
                                    const struct kefir_ast_initializer *initializer) {
    struct kefir_ast_node_base *head_expr = kefir_ast_initializer_head(initializer);
    if (head_expr != NULL && head_expr->properties.expression_props.string_literal != NULL &&
        is_char_array(type, NULL)) {
        REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDI64(builder, KEFIR_IROPCODE_PICK, 0));
        REQUIRE_OK(kefir_ast_translate_expression(mem, head_expr, builder, context));
        REQUIRE_OK(kefir_ast_translator_store_value(mem, type, context, builder));
    } else {
        REQUIRE(initializer->type == KEFIR_AST_INITIALIZER_LIST,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unable to initialize array by non-string literal expression"));

        REQUIRE_OK(zero_type(mem, context, builder, type));
        struct kefir_ast_type_traversal traversal;
        REQUIRE_OK(kefir_ast_type_traversal_init(mem, &traversal, type));
        kefir_result_t res = traverse_aggregate_union(mem, context, builder, type, initializer, &traversal);
        REQUIRE_OK(kefir_ast_type_traversal_free(mem, &traversal));
        REQUIRE_OK(res);
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_initializer(struct kefir_mem *mem,
                                           struct kefir_ast_translator_context *context,
                                           struct kefir_irbuilder_block *builder,
                                           const struct kefir_ast_type *type,
                                           const struct kefir_ast_initializer *initializer) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE(initializer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST initializer"));

    type = kefir_ast_unqualified_type(type);

    if (KEFIR_AST_TYPE_IS_SCALAR_TYPE(type)) {
        REQUIRE_OK(translate_scalar(mem, context, builder, type, initializer));
    } else if (type->tag == KEFIR_AST_TYPE_ARRAY) {
        REQUIRE_OK(translate_array(mem, context, builder, type, initializer));
    } else if (type->tag == KEFIR_AST_TYPE_STRUCTURE ||
               type->tag == KEFIR_AST_TYPE_UNION) {
        REQUIRE_OK(translate_struct_union(mem, context, builder, type, initializer));
    } else {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot initialize incomplete object type");
    }
    return KEFIR_OK;
}