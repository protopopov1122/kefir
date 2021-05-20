#include "kefir/ast/context_impl.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

kefir_result_t kefir_ast_context_free_scoped_identifier(struct kefir_mem *mem,
                                                    struct kefir_ast_scoped_identifier *scoped_id,
                                                    void *payload) {
    UNUSED(payload);
    if (scoped_id->cleanup.callback != NULL) {
        REQUIRE_OK(scoped_id->cleanup.callback(mem, scoped_id, scoped_id->cleanup.payload));
        scoped_id->cleanup.callback = NULL;
        scoped_id->cleanup.payload = NULL;
    }
    switch (scoped_id->klass) {
        case KEFIR_AST_SCOPE_IDENTIFIER_OBJECT:
            REQUIRE_OK(kefir_ast_alignment_free(mem, scoped_id->object.alignment));
            break;

        case KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT:
            REQUIRE_OK(kefir_ast_constant_expression_free(mem, scoped_id->enum_constant.value));
            break;

        default:
            break;
    }
    KEFIR_FREE(mem, scoped_id);
    return KEFIR_OK;
}

struct kefir_ast_scoped_identifier *kefir_ast_context_allocate_scoped_object_identifier(struct kefir_mem *mem,
                                                                                    const struct kefir_ast_type *type,
                                                                                    kefir_ast_scoped_identifier_storage_t storage,
                                                                                    struct kefir_ast_alignment *alignment,
                                                                                    kefir_ast_scoped_identifier_linkage_t linkage,
                                                                                    kefir_bool_t external,
                                                                                    struct kefir_ast_initializer *initializer) {
    struct kefir_ast_scoped_identifier *scoped_id = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_scoped_identifier));
    scoped_id->klass = KEFIR_AST_SCOPE_IDENTIFIER_OBJECT;
    scoped_id->cleanup.callback = NULL;
    scoped_id->cleanup.payload = NULL;
    scoped_id->object.type = type;
    scoped_id->object.storage = storage;
    scoped_id->object.external = external;
    scoped_id->object.linkage = linkage;
    scoped_id->object.initializer = initializer;
    memset(scoped_id->payload.content, 0, KEFIR_AST_SCOPED_IDENTIFIER_PAYLOAD_SIZE);
    scoped_id->payload.ptr = scoped_id->payload.content;
    scoped_id->payload.cleanup = &scoped_id->cleanup;
    if (alignment != NULL) {
        scoped_id->object.alignment = alignment;
    } else {
        scoped_id->object.alignment = kefir_ast_alignment_default(mem);
        REQUIRE_ELSE(scoped_id->object.alignment != NULL, {
            KEFIR_FREE(mem, scoped_id);
            return NULL;
        });
    }
    return scoped_id;
}

struct kefir_ast_scoped_identifier *kefir_ast_context_allocate_scoped_constant(struct kefir_mem *mem,
                                                                           struct kefir_ast_constant_expression *value,
                                                                           const struct kefir_ast_type *type) {
    struct kefir_ast_scoped_identifier *scoped_id = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_scoped_identifier));
    scoped_id->klass = KEFIR_AST_SCOPE_IDENTIFIER_ENUM_CONSTANT;
    scoped_id->cleanup.callback = NULL;
    scoped_id->cleanup.payload = NULL;
    scoped_id->enum_constant.type = type;
    scoped_id->enum_constant.value = value;
    memset(scoped_id->payload.content, 0, KEFIR_AST_SCOPED_IDENTIFIER_PAYLOAD_SIZE);
    scoped_id->payload.ptr = scoped_id->payload.content;
    scoped_id->payload.cleanup = &scoped_id->cleanup;
    return scoped_id;
}

struct kefir_ast_scoped_identifier *kefir_ast_context_allocate_scoped_type_tag(struct kefir_mem *mem,
                                                                           const struct kefir_ast_type *type) {
    struct kefir_ast_scoped_identifier *scoped_id = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_scoped_identifier));
    scoped_id->klass = KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG;
    scoped_id->cleanup.callback = NULL;
    scoped_id->cleanup.payload = NULL;
    scoped_id->type = type;
    memset(scoped_id->payload.content, 0, KEFIR_AST_SCOPED_IDENTIFIER_PAYLOAD_SIZE);
    scoped_id->payload.ptr = scoped_id->payload.content;
    scoped_id->payload.cleanup = &scoped_id->cleanup;
    return scoped_id;
}

struct kefir_ast_scoped_identifier *kefir_ast_context_allocate_scoped_type_definition(struct kefir_mem *mem,
                                                                                  const struct kefir_ast_type *type) {
    struct kefir_ast_scoped_identifier *scoped_id = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_scoped_identifier));
    scoped_id->klass = KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION;
    scoped_id->cleanup.callback = NULL;
    scoped_id->cleanup.payload = NULL;
    scoped_id->type = type;
    memset(scoped_id->payload.content, 0, KEFIR_AST_SCOPED_IDENTIFIER_PAYLOAD_SIZE);
    scoped_id->payload.ptr = scoped_id->payload.content;
    scoped_id->payload.cleanup = &scoped_id->cleanup;
    return scoped_id;
}

kefir_result_t kefir_ast_context_type_retrieve_tag(const struct kefir_ast_type *type,
                                               const char **identifier) {
    switch (type->tag) {
        case KEFIR_AST_TYPE_STRUCTURE:
        case KEFIR_AST_TYPE_UNION:
            REQUIRE(type->structure_type.identifier != NULL,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected struct/union with a tag"));
            *identifier = type->structure_type.identifier;
            return KEFIR_OK;

        case KEFIR_AST_TYPE_ENUMERATION:
            REQUIRE(type->enumeration_type.identifier != NULL,
                KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected enum with a tag"));
            *identifier = type->enumeration_type.identifier;
            return KEFIR_OK;
        
        default:
            return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected AST structure, union or enum type");
    }
}

kefir_result_t kefir_ast_context_update_existing_scoped_type_tag(struct kefir_ast_scoped_identifier *scoped_id,
                                                             const struct kefir_ast_type *type) {
    REQUIRE(scoped_id->klass == KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine with different kind of symbol"));
    REQUIRE(scoped_id->type->tag == type->tag,
        KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine tag with different type"));
    switch (scoped_id->type->tag) {
        case KEFIR_AST_TYPE_STRUCTURE:
        case KEFIR_AST_TYPE_UNION:
            if (type->structure_type.complete) {
                REQUIRE(!scoped_id->type->structure_type.complete,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine complete struct/union"));
                scoped_id->type = type;   
            }
            return KEFIR_OK;

        case KEFIR_AST_TYPE_ENUMERATION:
            if (type->enumeration_type.complete) {
                REQUIRE(!scoped_id->type->enumeration_type.complete,
                    KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Cannot redefine complete enumeration"));
                scoped_id->type = type;   
            }
            return KEFIR_OK;
        
        default:
            return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unexpected AST type");
    }
}


struct kefir_ast_scoped_identifier *kefir_ast_context_allocate_scoped_function_identifier(struct kefir_mem *mem,
                                                                                      const struct kefir_ast_type *type,
                                                                                      struct kefir_ast_function_specifier specifier,
                                                                                      kefir_ast_scoped_identifier_storage_t storage,
                                                                                      kefir_bool_t external) {
    struct kefir_ast_scoped_identifier *scoped_id = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_scoped_identifier));
    scoped_id->klass = KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION;
    scoped_id->cleanup.callback = NULL;
    scoped_id->cleanup.payload = NULL;
    scoped_id->function.type = type;
    scoped_id->function.specifier = specifier;
    scoped_id->function.storage = storage;
    scoped_id->function.external = external;
    memset(scoped_id->payload.content, 0, KEFIR_AST_SCOPED_IDENTIFIER_PAYLOAD_SIZE);
    scoped_id->payload.ptr = scoped_id->payload.content;
    scoped_id->payload.cleanup = &scoped_id->cleanup;
    return scoped_id;
}

struct kefir_ast_function_specifier kefir_ast_context_merge_function_specifiers(struct kefir_ast_function_specifier s1,
                                                                            struct kefir_ast_function_specifier s2) {
    return (struct kefir_ast_function_specifier) {
        .inline_function = s1.inline_function || s2.inline_function,
        .noreturn_function = s1.noreturn_function || s2.noreturn_function
    };
}

kefir_result_t kefir_ast_context_merge_alignment(struct kefir_mem *mem,
                                             struct kefir_ast_alignment **original,
                                             struct kefir_ast_alignment *alignment) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(original != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid original alignment pointer"));
    REQUIRE(alignment != NULL, KEFIR_OK);
    if (*original == NULL) {
        *original = alignment;
    } else if (alignment->value > (*original)->value) {
        REQUIRE_OK(kefir_ast_alignment_free(mem, *original));
        *original = alignment;
    } else {
        REQUIRE_OK(kefir_ast_alignment_free(mem, alignment));
    }
    return KEFIR_OK;
}
