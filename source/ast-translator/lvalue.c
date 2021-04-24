#include <string.h>
#include "kefir/ast-translator/translator.h"
#include "kefir/ast-translator/lvalue.h"
#include "kefir/ast-translator/temporaries.h"
#include "kefir/ast-translator/initializer.h"
#include "kefir/ast/runtime.h"
#include "kefir/ast/type_conv.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translator_object_lvalue(struct kefir_mem *mem,
                                              struct kefir_ast_translator_context *context,
                                              struct kefir_irbuilder_block *builder,
                                              const char *identifier,
                                              const struct kefir_ast_scoped_identifier *scoped_identifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(identifier != NULL && strlen(identifier) > 0,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid object identifier"));
    REQUIRE(scoped_identifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid scope identifier"));
    REQUIRE(scoped_identifier->klass == KEFIR_AST_SCOPE_IDENTIFIER_OBJECT,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected object scoped identifier"));

    switch (scoped_identifier->object.storage) {
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN: {
            kefir_id_t id;
            REQUIRE(kefir_ir_module_symbol(mem, context->module, identifier, &id) != NULL,
                 KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate IR module symbol"));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_GETGLOBAL,
                id));
        } break;

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC: {
            ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, identifier_data,
                scoped_identifier->payload.ptr);
            kefir_id_t id;
            REQUIRE(kefir_ir_module_symbol(mem, context->module,
                KEFIR_AST_TRANSLATOR_STATIC_VARIABLES_IDENTIFIER, &id) != NULL,
                KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate IR module symbol"));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_GETGLOBAL,
                id));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_OFFSETPTR,
                identifier_data->type_id, identifier_data->layout->value));

        } break;

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL:
            return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Thread_local variable support is not implemented yet");

        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO:
        case KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER: {
            ASSIGN_DECL_CAST(struct kefir_ast_translator_scoped_identifier_object *, identifier_data,
                scoped_identifier->payload.ptr);
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_GETLOCALS, 0));
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_OFFSETPTR,
                identifier_data->type_id, identifier_data->layout->value));
        } break;
    }

    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_function_lvalue(struct kefir_mem *mem,
                                                struct kefir_ast_translator_context *context,
                                                struct kefir_irbuilder_block *builder,
                                                const char *identifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(identifier != NULL && strlen(identifier) > 0,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid function identifier"));
        
    kefir_id_t id;
    REQUIRE(kefir_ir_module_symbol(mem, context->module, identifier, &id) != NULL,
        KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate IR module symbol"));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_GETGLOBAL,
        id));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_array_subscript_lvalue(struct kefir_mem *mem,
                                                      struct kefir_ast_translator_context *context,
                                                      struct kefir_irbuilder_block *builder,
                                                      const struct kefir_ast_array_subscript *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST array subscript node"));

    const struct kefir_ast_translator_resolved_type *cached_type = NULL;
    REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_OBJECT(mem, &context->type_resolver.resolver, context->environment, context->module,
        node->base.properties.type, 0, &cached_type));
    REQUIRE(cached_type->klass == KEFIR_AST_TRANSLATOR_RESOLVED_OBJECT_TYPE,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected cached type to be an object"));

    const struct kefir_ast_type *array_type = KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem,
        context->ast_context->type_bundle, node->array->properties.type);
    if (array_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->array, builder, context));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->subscript, builder, context));
    } else {
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->subscript, builder, context));
        REQUIRE_OK(kefir_ast_translate_expression(mem, node->array, builder, context));
    }
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_ELEMENTPTR, cached_type->object.ir_type_id, 0));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_struct_member_lvalue(struct kefir_mem *mem,
                                                    struct kefir_ast_translator_context *context,
                                                    struct kefir_irbuilder_block *builder,
                                                    const struct kefir_ast_struct_member *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST struct member node"));

    const struct kefir_ast_type *structure_type = KEFIR_AST_TYPE_CONV_EXPRESSION_ALL(mem, context->ast_context->type_bundle,
        node->structure->properties.type);
    if (structure_type->tag == KEFIR_AST_TYPE_SCALAR_POINTER) {
        structure_type = kefir_ast_unqualified_type(structure_type->referenced_type);
    }

    const struct kefir_ast_translator_resolved_type *cached_type = NULL;
    REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_OBJECT(mem, &context->type_resolver.resolver, context->environment, context->module,
        structure_type, 0, &cached_type));
    REQUIRE(cached_type->klass == KEFIR_AST_TRANSLATOR_RESOLVED_OBJECT_TYPE,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected cached type to be an object"));

    struct kefir_ast_type_layout *member_layout = NULL;
    struct kefir_ast_designator designator = {
        .type = KEFIR_AST_DESIGNATOR_MEMBER,
        .member = node->member,
        .next = NULL
    };
    REQUIRE_OK(kefir_ast_type_layout_resolve(cached_type->object.layout, &designator, &member_layout, NULL, NULL));

    REQUIRE_OK(kefir_ast_translate_expression(mem, node->structure, builder, context));
    REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_OFFSETPTR, cached_type->object.ir_type_id, member_layout->value));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_compound_literal_lvalue(struct kefir_mem *mem,
                                                       struct kefir_ast_translator_context *context,
                                                       struct kefir_irbuilder_block *builder,
                                                       const struct kefir_ast_compound_literal *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translation context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST compound literal node"));
    
    REQUIRE_OK(kefir_ast_translator_fetch_temporary(mem, context, builder,
        &node->base.properties.expression_props.temporary));
    REQUIRE_OK(kefir_ast_translate_initializer(mem, context, builder, node->base.properties.type, node->initializer));
    return KEFIR_OK;
}

struct translator_param {
    struct kefir_mem *mem;
    struct kefir_ast_translator_context *context;
    struct kefir_irbuilder_block *builder;
};

static kefir_result_t translate_not_impl(const struct kefir_ast_visitor *visitor,
                                       const struct kefir_ast_node_base *base,
                                       void *payload) {
    UNUSED(visitor);
    UNUSED(base);
    UNUSED(payload);
    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot translate non-lvalue AST node");
}

static kefir_result_t translate_identifier_node(const struct kefir_ast_visitor *visitor,
                                              const struct kefir_ast_identifier *node,
                                              void *payload) {
    REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST visitor"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct translator_param *, param,
        payload);

    const struct kefir_ast_scoped_identifier *scoped_identifier = NULL;
    REQUIRE_OK(param->context->ast_context->resolve_ordinary_identifier(param->context->ast_context, node->identifier,
        &scoped_identifier));
    switch (scoped_identifier->klass) {
        case KEFIR_AST_SCOPE_IDENTIFIER_OBJECT:
            REQUIRE_OK(kefir_ast_translator_object_lvalue(param->mem, param->context,
                param->builder, node->identifier, scoped_identifier));
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT:

        case KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION:
            REQUIRE_OK(kefir_ast_translator_function_lvalue(param->mem, param->context, param->builder,
                scoped_identifier->function.type->function_type.identifier));
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG:
        case KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Type definition is not an lvalue");

        case KEFIR_AST_SCOPE_IDENTIFIER_LABEL:
            return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Label is not an lvalue");

    }
    return KEFIR_OK;
    return KEFIR_OK;
}

static kefir_result_t translate_array_subscript_node(const struct kefir_ast_visitor *visitor,
                                                   const struct kefir_ast_array_subscript *node,
                                                   void *payload) {
    REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST visitor"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct translator_param *, param,
        payload);

    REQUIRE_OK(kefir_ast_translate_array_subscript_lvalue(param->mem, param->context, param->builder, node));
    return KEFIR_OK;
}

static kefir_result_t translate_struct_member_node(const struct kefir_ast_visitor *visitor,
                                                 const struct kefir_ast_struct_member *node,
                                                 void *payload) {
    REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST visitor"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct translator_param *, param,
        payload);

    REQUIRE_OK(kefir_ast_translate_struct_member_lvalue(param->mem, param->context, param->builder, node));
    return KEFIR_OK;
}

static kefir_result_t translate_unary_operation_node(const struct kefir_ast_visitor *visitor,
                                                   const struct kefir_ast_unary_operation *node,
                                                   void *payload) {
    REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST visitor"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct translator_param *, param,
        payload);

    switch (node->type) {
        case KEFIR_AST_OPERATION_INDIRECTION:
            REQUIRE_OK(kefir_ast_translate_expression(param->mem, node->arg, param->builder, param->context));
            break;

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Specified unary operator does not produce an lvalue");
    }
    return KEFIR_OK;
}

static kefir_result_t translate_compound_literal_node(const struct kefir_ast_visitor *visitor,
                                                    const struct kefir_ast_compound_literal *node,
                                                    void *payload) {
    REQUIRE(visitor != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST visitor"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid AST node"));
    REQUIRE(payload != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid payload"));
    ASSIGN_DECL_CAST(struct translator_param *, param,
        payload);

    REQUIRE_OK(kefir_ast_translate_compound_literal_lvalue(param->mem, param->context, param->builder, node));
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_lvalue(struct kefir_mem *mem,
                                      struct kefir_ast_translator_context *context,
                                      struct kefir_irbuilder_block *builder,
                                      const struct kefir_ast_node_base *node) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));
    REQUIRE(node != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST node"));

    struct kefir_ast_visitor visitor;
    REQUIRE_OK(kefir_ast_visitor_init(&visitor, translate_not_impl));
    visitor.identifier = translate_identifier_node;
    visitor.array_subscript = translate_array_subscript_node;
    visitor.struct_member = translate_struct_member_node;
    visitor.struct_indirect_member = translate_struct_member_node;
    visitor.unary_operation = translate_unary_operation_node;
    visitor.compound_literal = translate_compound_literal_node;
    struct translator_param param = {
        .mem = mem,
        .builder = builder,
        .context = context
    };
    return KEFIR_AST_NODE_VISIT(&visitor, node, &param);
}