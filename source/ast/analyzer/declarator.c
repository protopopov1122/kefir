#include "kefir/ast/declarator.h"
#include "kefir/ast/type.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

enum signedness {
    SIGNESS_DEFAULT,
    SIGNESS_SIGNED,
    SIGNESS_UNSIGNED
};

static kefir_result_t resolve_type(struct kefir_mem *mem,
                                 struct kefir_ast_type_bundle *type_bundle,
                                 enum signedness *signedness,
                                 const struct kefir_ast_type **base_type,
                                 const struct kefir_ast_type_specifier *specifier) {
    UNUSED(mem);
    UNUSED(type_bundle);
    switch (specifier->specifier) {
        case KEFIR_AST_TYPE_SPECIFIER_VOID:
            REQUIRE(*base_type == NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Void type specifier cannot be combined with others"));
            *base_type = kefir_ast_type_void();
            break;

        case KEFIR_AST_TYPE_SPECIFIER_CHAR:
            REQUIRE(*base_type == NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Char type specifier cannot be combined with others"));
            *base_type = kefir_ast_type_char();
            break;
            
        case KEFIR_AST_TYPE_SPECIFIER_SHORT:
            REQUIRE(*base_type == NULL ||
                (*base_type)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_INT,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Short type specifier can only be combined with int"));
            *base_type = kefir_ast_type_signed_short();
            break;

        case KEFIR_AST_TYPE_SPECIFIER_INT:
            if (*base_type == NULL || (*base_type)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT) {
                *base_type = kefir_ast_type_signed_int();
            } else {
                REQUIRE((*base_type)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_LONG ||
                    (*base_type)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Int type specifier can only be combined with short or long"));
            }
            break;
            
        case KEFIR_AST_TYPE_SPECIFIER_LONG:
            if (*base_type != NULL || (*base_type)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_LONG) {
                *base_type = kefir_ast_type_signed_long_long();
            } else {
                REQUIRE((*base_type)->tag == KEFIR_AST_TYPE_SCALAR_SIGNED_INT,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Long type specifier can only be combined with int or long"));
                *base_type = kefir_ast_type_signed_long();
            }
            break;

        case KEFIR_AST_TYPE_SPECIFIER_FLOAT:
            REQUIRE(*base_type == NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Float type specifier cannot be combined with others"));
            *base_type = kefir_ast_type_float();
            break;

        case KEFIR_AST_TYPE_SPECIFIER_DOUBLE:
            REQUIRE(*base_type == NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Double type specifier cannot be combined with others"));
            *base_type = kefir_ast_type_double();
            break;

        case KEFIR_AST_TYPE_SPECIFIER_SIGNED:
            REQUIRE(*signedness == SIGNESS_DEFAULT,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Signed type specifier cannot be combined with other sigedness specifiers"));
            *signedness = SIGNESS_SIGNED;
            break;

        case KEFIR_AST_TYPE_SPECIFIER_UNSIGNED:
            REQUIRE(*signedness == SIGNESS_DEFAULT,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unsigned type specifier cannot be combined with other sigedness specifiers"));
            *signedness = SIGNESS_UNSIGNED;
            break;

        case KEFIR_AST_TYPE_SPECIFIER_BOOL:
            REQUIRE(*base_type == NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Boolean type specifier cannot be combined with others"));
            *base_type = kefir_ast_type_bool();
            break;

        case KEFIR_AST_TYPE_SPECIFIER_COMPLEX:
            return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, "Complex floating-point type specifier is not supported yet");

        case KEFIR_AST_TYPE_SPECIFIER_ATOMIC:
            return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, "Atomic type specifier is not supported yet");
            
        case KEFIR_AST_TYPE_SPECIFIER_STRUCT:
        case KEFIR_AST_TYPE_SPECIFIER_UNION:
        case KEFIR_AST_TYPE_SPECIFIER_ENUM:
        case KEFIR_AST_TYPE_SPECIFIER_TYPEDEF:
            return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Complex type specifiers are not implemented yet");

        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected type specifier");
    }
    return KEFIR_OK;
}

static kefir_result_t apply_type_sigedness(struct kefir_mem *mem,
                                         struct kefir_ast_type_bundle *type_bundle,
                                         enum signedness signedness,
                                         const struct kefir_ast_type **base_type) {
    UNUSED(mem);
    UNUSED(type_bundle);
    if (signedness == SIGNESS_DEFAULT) {
        if ((*base_type) == NULL) {
            (*base_type) = kefir_ast_type_signed_int();
        }
    } else if (signedness == SIGNESS_SIGNED) {
        if ((*base_type) == NULL) {
            (*base_type) = kefir_ast_type_signed_int();
        } else {
            switch ((*base_type)->tag) {
                case KEFIR_AST_TYPE_SCALAR_CHAR:
                    (*base_type) = kefir_ast_type_signed_char();
                    break;

                case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
                case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
                case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
                case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
                    // Nothing to be done
                    break;

                case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
                    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected AST type");

                case KEFIR_AST_TYPE_VOID:
                case KEFIR_AST_TYPE_SCALAR_BOOL:
                case KEFIR_AST_TYPE_SCALAR_FLOAT:
                case KEFIR_AST_TYPE_SCALAR_DOUBLE:
                case KEFIR_AST_TYPE_SCALAR_POINTER:
                case KEFIR_AST_TYPE_ENUMERATION:
                case KEFIR_AST_TYPE_STRUCTURE:
                case KEFIR_AST_TYPE_UNION:
                case KEFIR_AST_TYPE_ARRAY:
                case KEFIR_AST_TYPE_FUNCTION:
                case KEFIR_AST_TYPE_QUALIFIED:
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Signed type specifier cannot be applied to the type");
            }
        }
    } else {
        if ((*base_type) == NULL) {
            (*base_type) = kefir_ast_type_unsigned_int();
        } else {
            switch ((*base_type)->tag) {
                case KEFIR_AST_TYPE_SCALAR_CHAR:
                    (*base_type) = kefir_ast_type_unsigned_char();
                    break;

                case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
                    (*base_type) = kefir_ast_type_unsigned_short();
                    break;

                case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
                    (*base_type) = kefir_ast_type_unsigned_int();
                    break;

                case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
                    (*base_type) = kefir_ast_type_unsigned_long();
                    break;

                case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
                    (*base_type) = kefir_ast_type_unsigned_long_long();
                    break;

                case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
                case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
                    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected AST type");

                case KEFIR_AST_TYPE_VOID:
                case KEFIR_AST_TYPE_SCALAR_BOOL:
                case KEFIR_AST_TYPE_SCALAR_FLOAT:
                case KEFIR_AST_TYPE_SCALAR_DOUBLE:
                case KEFIR_AST_TYPE_SCALAR_POINTER:
                case KEFIR_AST_TYPE_ENUMERATION:
                case KEFIR_AST_TYPE_STRUCTURE:
                case KEFIR_AST_TYPE_UNION:
                case KEFIR_AST_TYPE_ARRAY:
                case KEFIR_AST_TYPE_FUNCTION:
                case KEFIR_AST_TYPE_QUALIFIED:
                    return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unsigned type specifier cannot be applied to the type");
            }
        }
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_analyze_declaration(struct kefir_mem *mem,
                                         struct kefir_ast_type_bundle *type_bundle,
                                         const struct kefir_ast_declarator_specifier_list *specifiers,
                                         const struct kefir_ast_declarator *declarator,
                                         const struct kefir_ast_type **type,
                                         kefir_ast_scoped_identifier_storage_t *storage,
                                         struct kefir_ast_function_specifier *function) {
    UNUSED(storage);
    UNUSED(function);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(specifiers != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST declarator specifier list"));
    REQUIRE(declarator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST declarator"));

    enum signedness signedness = SIGNESS_DEFAULT;
    const struct kefir_ast_type *base_type = NULL;

    struct kefir_ast_declarator_specifier *declatator_specifier;
    for (struct kefir_list_entry *iter = kefir_ast_declarator_specifier_list_iter(specifiers, &declatator_specifier);
        iter != NULL;
        kefir_ast_declarator_specifier_list_next(&iter, &declatator_specifier)) {
        switch (declatator_specifier->klass) {
            case KEFIR_AST_TYPE_SPECIFIER:
                REQUIRE_OK(resolve_type(mem, type_bundle, &signedness, &base_type, &declatator_specifier->type_specifier));
                break;
            
            case KEFIR_AST_TYPE_QUALIFIER:
            case KEFIR_AST_STORAGE_CLASS_SPECIFIER:
            case KEFIR_AST_FUNCTION_SPECIFIER:
            case KEFIR_AST_ALIGNMENT_SPECIFIER:
                return KEFIR_SET_ERROR(KEFIR_NOT_IMPLEMENTED, "Other types of declarator specifiers are not implemented yet");
        }
    }
    REQUIRE_OK(apply_type_sigedness(mem, type_bundle, signedness, &base_type));
    ASSIGN_PTR(type, base_type);
    return KEFIR_OK;
}
