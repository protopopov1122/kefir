#include "kefir/ast/analyzer/analyzer.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

static kefir_result_t analyze_enum(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                   const struct kefir_ast_enum_type *enum_type) {
    REQUIRE_OK(kefir_ast_analyze_type(mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, enum_type->underlying_type));
    if (enum_type->complete) {
        kefir_int64_t next_value = 0;
        for (const struct kefir_list_entry *iter = kefir_list_head(&enum_type->enumerators); iter != NULL;
             kefir_list_next(&iter)) {

            ASSIGN_DECL_CAST(struct kefir_ast_enum_enumerator *, enumerator, iter->value);
            if (enumerator->value != NULL) {
                REQUIRE_OK(kefir_ast_analyze_constant_expression(mem, context, enumerator->value));
                REQUIRE_OK(kefir_ast_constant_expression_evaluate(mem, context, enumerator->value));
                REQUIRE(
                    enumerator->value->value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Enumerator constant expression shall have integral type"));
                next_value = enumerator->value->value.integer + 1;
            } else {
                enumerator->value = kefir_ast_constant_expression_integer(mem, next_value);
                REQUIRE(enumerator->value != NULL,
                        KEFIR_SET_ERROR(KEFIR_MEMALLOC_FAILURE, "Failed to allocate constant expression"));
                next_value++;
            }
        }
    }
    return KEFIR_OK;
}

static kefir_result_t analyze_array(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                    kefir_ast_type_analysis_context_t analysis_context,
                                    const struct kefir_ast_array_type *array_type) {
    REQUIRE_OK(kefir_ast_analyze_type(mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, array_type->element_type));
    REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(array_type->element_type) &&
                array_type->element_type->tag != KEFIR_AST_TYPE_FUNCTION,
            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Array element type shall be complete non-function type"));
    REQUIRE(
        (array_type->boundary != KEFIR_AST_ARRAY_BOUNDED_STATIC && array_type->boundary != KEFIR_AST_ARRAY_VLA_STATIC &&
         KEFIR_AST_TYPE_IS_ZERO_QUALIFICATION(&array_type->qualifications)) ||
            analysis_context == KEFIR_AST_TYPE_ANALYSIS_FUNCTION_PARAMETER,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                        "Array type qualifications shall appear only for the outermost function parameter type"));
    switch (array_type->boundary) {
        case KEFIR_AST_ARRAY_UNBOUNDED:
            break;

        case KEFIR_AST_ARRAY_BOUNDED:
        case KEFIR_AST_ARRAY_BOUNDED_STATIC:
            REQUIRE_OK(kefir_ast_analyze_constant_expression(mem, context, array_type->const_length));
            REQUIRE_OK(kefir_ast_constant_expression_evaluate(mem, context, array_type->const_length));
            REQUIRE(array_type->const_length->value.klass == KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Array type subscript shall have integral type"));
            break;

        case KEFIR_AST_ARRAY_VLA:
        case KEFIR_AST_ARRAY_VLA_STATIC:
            if (array_type->vla_length != NULL) {
                REQUIRE_OK(kefir_ast_analyze_node(mem, context, array_type->vla_length));
                REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(array_type->vla_length->properties.type),
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Array type subscript shall have integral type"));
            }
            break;
    }
    return KEFIR_OK;
}

static kefir_result_t analyze_structure(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                        const struct kefir_ast_type *type) {
    const struct kefir_ast_struct_type *struct_type = &type->structure_type;
    if (struct_type->complete) {
        for (const struct kefir_list_entry *iter = kefir_list_head(&struct_type->fields); iter != NULL;
             kefir_list_next(&iter)) {
            ASSIGN_DECL_CAST(struct kefir_ast_struct_field *, field, iter->value);
            REQUIRE_OK(kefir_ast_analyze_type(mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, field->type));
            REQUIRE_OK(kefir_ast_analyze_alignment(mem, context, field->alignment));
            REQUIRE_OK(kefir_ast_alignment_evaluate(mem, context, field->alignment));
            if (field->bitfield) {
                REQUIRE_OK(kefir_ast_analyze_constant_expression(mem, context, field->bitwidth));
                if (field->bitwidth->expression != NULL) {
                    REQUIRE(KEFIR_AST_TYPE_IS_INTEGRAL_TYPE(field->bitwidth->expression->properties.type),
                            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Bit-field shall have integral type"));
                }
                REQUIRE_OK(kefir_ast_constant_expression_evaluate(mem, context, field->bitwidth));
                REQUIRE(field->bitwidth->value.integer >= 0,
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Structure/union bitfield width shall be non-negative"));
                REQUIRE(field->bitwidth->value.integer > 0 || field->identifier == NULL,
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Named bit-field with zero width is not permitted"));
                REQUIRE(field->alignment->klass == KEFIR_AST_ALIGNMENT_DEFAULT,
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Bit-field shall not have alignment attribute"));
            }

            if (field->type->tag == KEFIR_AST_TYPE_ARRAY) {
                if (field->type->array_type.boundary == KEFIR_AST_ARRAY_UNBOUNDED) {
                    REQUIRE(
                        iter->next == NULL && type->tag == KEFIR_AST_TYPE_STRUCTURE,
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Flexible member should be the last member of structure"));
                    REQUIRE(iter->prev != NULL,
                            KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                            "Flexible member is not allowed in otherwise empty structure"));
                } else {
                    REQUIRE(
                        !KEFIR_AST_TYPE_IS_VARIABLY_MODIFIED(field->type),
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Structure/union cannot have variably modified members"));
                }
            } else {
                REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(field->type) && field->type->tag != KEFIR_AST_TYPE_FUNCTION,
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG,
                                        "Structure/union field shall have complete non-function type"));
            }
        }
    }
    return KEFIR_OK;
}

static kefir_result_t analyze_function(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                       const struct kefir_ast_function_type *func_type) {
    REQUIRE(
        func_type->return_type->tag != KEFIR_AST_TYPE_FUNCTION && func_type->return_type->tag != KEFIR_AST_TYPE_ARRAY,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function return type shall not be array or function"));

    if (func_type->mode == KEFIR_AST_FUNCTION_TYPE_PARAMETERS) {
        for (const struct kefir_list_entry *iter = kefir_list_head(&func_type->parameters); iter != NULL;
             kefir_list_next(&iter)) {

            ASSIGN_DECL_CAST(struct kefir_ast_function_type_parameter *, param, iter->value);

            REQUIRE_OK(
                kefir_ast_analyze_type(mem, context, KEFIR_AST_TYPE_ANALYSIS_FUNCTION_PARAMETER, param->adjusted_type));
            if (param->adjusted_type->tag == KEFIR_AST_TYPE_VOID) {
                REQUIRE(param->identifier == NULL,
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Void type function parameter shall be unnamed"));
                REQUIRE(
                    kefir_list_length(&func_type->parameters) == 1,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Void type parameter shall be the only function parameter"));
            } else {
                REQUIRE(!KEFIR_AST_TYPE_IS_INCOMPLETE(param->adjusted_type),
                        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function parameters shall not have incomplete type"));
                REQUIRE(
                    KEFIR_OPTIONAL_EMPTY(&param->storage) ||
                        *KEFIR_OPTIONAL_VALUE(&param->storage) == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER ||
                        *KEFIR_OPTIONAL_VALUE(&param->storage) == KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_UNKNOWN,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Function parameter might only have register storage class"));
            }
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_analyze_type(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                      kefir_ast_type_analysis_context_t analysis_context,
                                      const struct kefir_ast_type *type) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));

    switch (type->tag) {
        case KEFIR_AST_TYPE_VOID:
        case KEFIR_AST_TYPE_SCALAR_BOOL:
        case KEFIR_AST_TYPE_SCALAR_CHAR:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_FLOAT:
        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
            // No analysis needed for arithmetic types
            break;

        case KEFIR_AST_TYPE_SCALAR_POINTER:
            REQUIRE_OK(kefir_ast_analyze_type(mem, context, KEFIR_AST_TYPE_ANALYSIS_DEFAULT, type->referenced_type));
            break;

        case KEFIR_AST_TYPE_ENUMERATION:
            REQUIRE_OK(analyze_enum(mem, context, &type->enumeration_type));
            break;

        case KEFIR_AST_TYPE_STRUCTURE:
        case KEFIR_AST_TYPE_UNION:
            REQUIRE_OK(analyze_structure(mem, context, type));
            break;

        case KEFIR_AST_TYPE_ARRAY:
            REQUIRE_OK(analyze_array(mem, context, analysis_context, &type->array_type));
            break;

        case KEFIR_AST_TYPE_FUNCTION:
            REQUIRE_OK(analyze_function(mem, context, &type->function_type));
            break;

        case KEFIR_AST_TYPE_QUALIFIED:
            REQUIRE_OK(kefir_ast_analyze_type(mem, context, analysis_context, type->qualified_type.type));
            break;
    }

    return KEFIR_OK;
}
