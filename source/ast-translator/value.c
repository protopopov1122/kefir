#include "kefir/ast-translator/value.h"
#include "kefir/ast-translator/util.h"
#include "kefir/ast-translator/type_resolver.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_translator_load_value(const struct kefir_ast_type *type,
                                           const struct kefir_ast_type_traits *type_traits,
                                           struct kefir_irbuilder_block *builder) {
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));

    const struct kefir_ast_type *normalizer = kefir_ast_translator_normalize_type(type);

    switch (normalizer->tag) {
        case KEFIR_AST_TYPE_VOID:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot load variable with void type");
            
        case KEFIR_AST_TYPE_SCALAR_BOOL:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD8U, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_CHAR:
            if (type_traits->character_type_signedness) {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD8I, 0));
            } else {
                REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD8U, 0));
            }
            break;
            
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD8U, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD8I, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD16U, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD16I, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD32U, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD32I, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
        case KEFIR_AST_TYPE_SCALAR_POINTER:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_LOAD64, 0));
            break;
            
        case KEFIR_AST_TYPE_ENUMERATION:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected enumeration type");
            
        case KEFIR_AST_TYPE_STRUCTURE:
        case KEFIR_AST_TYPE_UNION:
        case KEFIR_AST_TYPE_ARRAY:
        case KEFIR_AST_TYPE_FUNCTION:
            // Intentionally left blank
            break;
            
        case KEFIR_AST_TYPE_QUALIFIED:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected qualified type");
            
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translator_store_value(struct kefir_mem *mem,
                                            const struct kefir_ast_type *type,
                                            struct kefir_ast_translator_context *context,
                                            struct kefir_irbuilder_block *builder) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST translator context"));
    REQUIRE(builder != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR block builder"));

    const struct kefir_ast_type *normalizer = kefir_ast_translator_normalize_type(type);

    switch (normalizer->tag) {
        case KEFIR_AST_TYPE_VOID:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot store value with void type");
            
        case KEFIR_AST_TYPE_SCALAR_BOOL:
        case KEFIR_AST_TYPE_SCALAR_CHAR:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_CHAR:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_CHAR:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE8, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_SHORT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_SHORT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE16, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_INT:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_INT:
        case KEFIR_AST_TYPE_SCALAR_FLOAT:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE32, 0));
            break;
            
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_UNSIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG:
        case KEFIR_AST_TYPE_SCALAR_SIGNED_LONG_LONG:
        case KEFIR_AST_TYPE_SCALAR_DOUBLE:
        case KEFIR_AST_TYPE_SCALAR_POINTER:
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU64(builder, KEFIR_IROPCODE_STORE64, 0));
            break;
            
        case KEFIR_AST_TYPE_ENUMERATION:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected enumeration type");
            
        case KEFIR_AST_TYPE_STRUCTURE:
        case KEFIR_AST_TYPE_UNION:
        case KEFIR_AST_TYPE_ARRAY: {
            const struct kefir_ast_translator_resolved_type *cached_type = NULL;
            REQUIRE_OK(KEFIR_AST_TRANSLATOR_TYPE_RESOLVER_BUILD_OBJECT(mem, &context->type_cache.resolver, context->environment, context->module,
                type, 0, &cached_type));
            REQUIRE(cached_type->klass == KEFIR_AST_TRANSLATOR_RESOLVED_OBJECT_TYPE,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected cached type to be an object"));
            
            REQUIRE_OK(KEFIR_IRBUILDER_BLOCK_APPENDU32(builder, KEFIR_IROPCODE_BCOPY,
                cached_type->object.ir_type_id, cached_type->object.layout->value));
        } break;

        case KEFIR_AST_TYPE_FUNCTION:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot store value with function type");
            
        case KEFIR_AST_TYPE_QUALIFIED:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected qualified type");
            
    }
    return KEFIR_OK;
}