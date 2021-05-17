#include "kefir/ast/declarator.h"
#include "kefir/ast/node.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

#define TYPE_SPECIFIER(_id, _spec) \
struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_##_id(struct kefir_mem *mem) { \
    REQUIRE(mem != NULL, NULL); \
    struct kefir_ast_declarator_specifier *specifier = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier)); \
    REQUIRE(specifier != NULL, NULL); \
 \
    specifier->klass = KEFIR_AST_TYPE_SPECIFIER; \
    specifier->type_specifier.specifier = (_spec); \
    return specifier; \
}

TYPE_SPECIFIER(void, KEFIR_AST_TYPE_SPECIFIER_VOID)
TYPE_SPECIFIER(char, KEFIR_AST_TYPE_SPECIFIER_CHAR)
TYPE_SPECIFIER(short, KEFIR_AST_TYPE_SPECIFIER_SHORT)
TYPE_SPECIFIER(int, KEFIR_AST_TYPE_SPECIFIER_INT)
TYPE_SPECIFIER(long, KEFIR_AST_TYPE_SPECIFIER_LONG)
TYPE_SPECIFIER(float, KEFIR_AST_TYPE_SPECIFIER_FLOAT)
TYPE_SPECIFIER(double, KEFIR_AST_TYPE_SPECIFIER_DOUBLE)
TYPE_SPECIFIER(signed, KEFIR_AST_TYPE_SPECIFIER_SIGNED)
TYPE_SPECIFIER(unsigned, KEFIR_AST_TYPE_SPECIFIER_UNSIGNED)
TYPE_SPECIFIER(bool, KEFIR_AST_TYPE_SPECIFIER_BOOL)
TYPE_SPECIFIER(complex, KEFIR_AST_TYPE_SPECIFIER_COMPLEX)

#undef TYPE_SPECIFIER

struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_atomic(struct kefir_mem *mem, struct kefir_ast_node_base *type) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(type != NULL, NULL);

    struct kefir_ast_declarator_specifier *specifier = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier));
    REQUIRE(specifier != NULL, NULL);
    
    specifier->klass = KEFIR_AST_TYPE_SPECIFIER;
    specifier->type_specifier.specifier = KEFIR_AST_TYPE_SPECIFIER_ATOMIC;
    specifier->type_specifier.value.atomic_type = type;
    return specifier;
}

struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_struct(struct kefir_mem *mem, struct kefir_ast_structure_specifier *structure) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(structure != NULL, NULL);

    struct kefir_ast_declarator_specifier *specifier = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier));
    REQUIRE(specifier != NULL, NULL);
    
    specifier->klass = KEFIR_AST_TYPE_SPECIFIER;
    specifier->type_specifier.specifier = KEFIR_AST_TYPE_SPECIFIER_STRUCT;
    specifier->type_specifier.value.structure = structure;
    return specifier;
}

struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_union(struct kefir_mem *mem, struct kefir_ast_structure_specifier *structure) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(structure != NULL, NULL);

    struct kefir_ast_declarator_specifier *specifier = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier));
    REQUIRE(specifier != NULL, NULL);
    
    specifier->klass = KEFIR_AST_TYPE_SPECIFIER;
    specifier->type_specifier.specifier = KEFIR_AST_TYPE_SPECIFIER_UNION;
    specifier->type_specifier.value.structure = structure;
    return specifier;
}

struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_enum(struct kefir_mem *mem, struct kefir_ast_enum_specifier *enumeration) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(enumeration != NULL, NULL);

    struct kefir_ast_declarator_specifier *specifier = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier));
    REQUIRE(specifier != NULL, NULL);
    
    specifier->klass = KEFIR_AST_TYPE_SPECIFIER;
    specifier->type_specifier.specifier = KEFIR_AST_TYPE_SPECIFIER_ENUM;
    specifier->type_specifier.value.enumeration = enumeration;
    return specifier;
}

struct kefir_ast_declarator_specifier *kefir_ast_type_specifier_typedef(struct kefir_mem *mem,
                                                                    struct kefir_symbol_table *symbols,
                                                                    const char *literal) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(literal != NULL, NULL);

    if (symbols != NULL) {
        literal = kefir_symbol_table_insert(mem, symbols, literal, NULL);
        REQUIRE(literal != NULL, NULL);
    }

    struct kefir_ast_declarator_specifier *specifier = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier));
    REQUIRE(specifier != NULL, NULL);
    
    specifier->klass = KEFIR_AST_TYPE_SPECIFIER;
    specifier->type_specifier.specifier = KEFIR_AST_TYPE_SPECIFIER_TYPEDEF;
    specifier->type_specifier.value.type_name = literal;
    return specifier;
}

#define STORAGE_CLASS_SPECIFIER(_id, _spec) \
struct kefir_ast_declarator_specifier *kefir_ast_storage_class_specifier_##_id(struct kefir_mem *mem) { \
    REQUIRE(mem != NULL, NULL); \
    struct kefir_ast_declarator_specifier *specifier = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier)); \
    REQUIRE(specifier != NULL, NULL); \
 \
    specifier->klass = KEFIR_AST_STORAGE_CLASS_SPECIFIER; \
    specifier->storage_class = (_spec); \
    return specifier; \
}

STORAGE_CLASS_SPECIFIER(typedef, KEFIR_AST_STORAGE_SPECIFIER_TYPEDEF)
STORAGE_CLASS_SPECIFIER(extern, KEFIR_AST_STORAGE_SPECIFIER_EXTERN)
STORAGE_CLASS_SPECIFIER(static, KEFIR_AST_STORAGE_SPECIFIER_STATIC)
STORAGE_CLASS_SPECIFIER(thread_local, KEFIR_AST_STORAGE_SPECIFIER_THREAD_LOCAL)
STORAGE_CLASS_SPECIFIER(auto, KEFIR_AST_STORAGE_SPECIFIER_AUTO)
STORAGE_CLASS_SPECIFIER(register, KEFIR_AST_STORAGE_SPECIFIER_REGISTER)

#undef STORAGE_CLASS_SPECIFIER

#define TYPE_QUALIFIER(_id, _spec) \
struct kefir_ast_declarator_specifier *kefir_ast_type_qualifier_##_id(struct kefir_mem *mem) { \
    REQUIRE(mem != NULL, NULL); \
    struct kefir_ast_declarator_specifier *specifier = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier)); \
    REQUIRE(specifier != NULL, NULL); \
 \
    specifier->klass = KEFIR_AST_TYPE_QUALIFIER; \
    specifier->type_qualifier = (_spec); \
    return specifier; \
}

TYPE_QUALIFIER(const, KEFIR_AST_TYPE_QUALIFIER_CONST)
TYPE_QUALIFIER(restrict, KEFIR_AST_TYPE_QUALIFIER_RESTRICT)
TYPE_QUALIFIER(volatile, KEFIR_AST_TYPE_QUALIFIER_VOLATILE)
TYPE_QUALIFIER(atomic, KEFIR_AST_TYPE_QUALIFIER_ATOMIC)

#undef TYPE_QUALIFIER

#define FUNCTION_SPECIFIER(_id, _spec) \
struct kefir_ast_declarator_specifier *kefir_ast_function_specifier_##_id(struct kefir_mem *mem) { \
    REQUIRE(mem != NULL, NULL); \
    struct kefir_ast_declarator_specifier *specifier = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier)); \
    REQUIRE(specifier != NULL, NULL); \
 \
    specifier->klass = KEFIR_AST_FUNCTION_SPECIFIER; \
    specifier->function_specifier = (_spec); \
    return specifier; \
}

FUNCTION_SPECIFIER(inline, KEFIR_AST_FUNCTION_SPECIFIER_TYPE_INLINE)
FUNCTION_SPECIFIER(noreturn, KEFIR_AST_FUNCTION_SPECIFIER_TYPE_NORETURN)

#undef FUNCTION_SPECIFIER

struct kefir_ast_declarator_specifier *kefir_ast_alignment_specifier(struct kefir_mem *mem, struct kefir_ast_node_base *alignment) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(alignment != NULL, NULL);

    struct kefir_ast_declarator_specifier *specifier = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator_specifier));
    REQUIRE(specifier != NULL, NULL);

    specifier->klass = KEFIR_AST_ALIGNMENT_SPECIFIER;
    specifier->alignment_specifier = alignment;
    return specifier;
}

kefir_result_t kefir_ast_declarator_specifier_free(struct kefir_mem *mem, struct kefir_ast_declarator_specifier *specifier) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(specifier != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST declarator specifier"));

    switch (specifier->klass) {
        case KEFIR_AST_TYPE_SPECIFIER:
            switch (specifier->type_specifier.specifier) {
                case KEFIR_AST_TYPE_SPECIFIER_ATOMIC:
                    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, specifier->type_specifier.value.atomic_type));
                    specifier->type_specifier.value.atomic_type = NULL;
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_STRUCT:
                case KEFIR_AST_TYPE_SPECIFIER_UNION:
                    // TODO Implement structure specifier freeing
                    break;

                case KEFIR_AST_TYPE_SPECIFIER_ENUM:
                    // TODO Implement structure specifier freeing
                    break;

                default:
                    // Nothing to do
                    break;
            }
            break;

        case KEFIR_AST_ALIGNMENT_SPECIFIER:
            REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, specifier->alignment_specifier));
            specifier->alignment_specifier = NULL;
            break;

        case KEFIR_AST_TYPE_QUALIFIER:
        case KEFIR_AST_STORAGE_CLASS_SPECIFIER:
        case KEFIR_AST_FUNCTION_SPECIFIER:
            // Nothing to do
            break;
    }

    KEFIR_FREE(mem, specifier);
    return KEFIR_OK;
}

struct kefir_ast_declarator *kefir_ast_declarator_identifier(struct kefir_mem *mem,
                                                         struct kefir_symbol_table *symbols,
                                                         const char *identifier) {
    REQUIRE(mem != NULL, NULL);

    if (symbols != NULL && identifier != NULL) {
        identifier = kefir_symbol_table_insert(mem, symbols, identifier, NULL);
        REQUIRE(identifier != NULL, NULL);
    }

    struct kefir_ast_declarator *decl = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator));
    REQUIRE(decl != NULL, NULL);
    decl->klass = KEFIR_AST_DECLARATOR_IDENTIFIER;
    decl->identifier = identifier;
    return decl;
}

struct kefir_ast_declarator *kefir_ast_declarator_pointer(struct kefir_mem *mem, struct kefir_ast_declarator *direct) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(direct != NULL, NULL);

    struct kefir_ast_declarator *decl = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator));
    REQUIRE(decl != NULL, NULL);
    decl->klass = KEFIR_AST_DECLARATOR_POINTER;

    kefir_result_t res = kefir_list_init(&decl->pointer.type_qualifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, decl);
        return NULL;
    });

    decl->pointer.declarator = direct;
    return decl;
}

struct kefir_ast_declarator *kefir_ast_declarator_array(struct kefir_mem *mem,
                                                    struct kefir_ast_node_base *length,
                                                    struct kefir_ast_declarator *direct) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(direct != NULL, NULL);

    struct kefir_ast_declarator *decl = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator));
    REQUIRE(decl != NULL, NULL);
    decl->klass = KEFIR_AST_DECLARATOR_ARRAY;

    kefir_result_t res = kefir_list_init(&decl->array.type_qualifiers);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, decl);
        return NULL;
    });
    
    decl->array.length = length;
    decl->array.static_array = false;
    decl->array.declarator = direct;
    return decl;
}

static kefir_result_t free_function_param(struct kefir_mem *mem,
                                        struct kefir_list *list,
                                        struct kefir_list_entry *entry,
                                        void *payload) {
    UNUSED(list);
    UNUSED(payload);
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(entry != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid list entry"));
    ASSIGN_DECL_CAST(struct kefir_ast_node_base *, param,
        entry->value);
    REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, param));
    return KEFIR_OK;
}

struct kefir_ast_declarator *kefir_ast_declarator_function(struct kefir_mem *mem, struct kefir_ast_declarator *direct) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(direct != NULL, NULL);

    struct kefir_ast_declarator *decl = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_declarator));
    REQUIRE(decl != NULL, NULL);
    decl->klass = KEFIR_AST_DECLARATOR_FUNCTION;

    kefir_result_t res = kefir_list_init(&decl->function.parameters);
    REQUIRE_ELSE(res == KEFIR_OK, {
        KEFIR_FREE(mem, decl);
        return NULL;
    });

    res = kefir_list_on_remove(&decl->function.parameters, free_function_param, NULL);
    REQUIRE_ELSE(res == KEFIR_OK, {
        kefir_list_free(mem, &decl->function.parameters);
        KEFIR_FREE(mem, decl);
        return NULL;
    });
    
    decl->function.declarator = direct;
    return decl;
}

kefir_result_t kefir_ast_declarator_free(struct kefir_mem *mem, struct kefir_ast_declarator *decl) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(decl != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST declarator"));

    switch (decl->klass) {
        case KEFIR_AST_DECLARATOR_IDENTIFIER:
            decl->identifier = NULL;
            break;

        case KEFIR_AST_DECLARATOR_POINTER:
            REQUIRE_OK(kefir_list_free(mem, &decl->pointer.type_qualifiers));
            REQUIRE_OK(kefir_ast_declarator_free(mem, decl->pointer.declarator));
            decl->pointer.declarator = NULL;
            break;

        case KEFIR_AST_DECLARATOR_ARRAY:
            REQUIRE_OK(kefir_list_free(mem, &decl->array.type_qualifiers));
            REQUIRE_OK(KEFIR_AST_NODE_FREE(mem, decl->array.length));
            decl->array.length = NULL;
            decl->array.static_array = false;
            REQUIRE_OK(kefir_ast_declarator_free(mem, decl->array.declarator));
            decl->array.declarator = NULL;
            break;

        case KEFIR_AST_DECLARATOR_FUNCTION:
            REQUIRE_OK(kefir_list_free(mem, &decl->function.parameters));
            REQUIRE_OK(kefir_ast_declarator_free(mem, decl->function.declarator));
            decl->function.declarator = NULL;
            break;

    }
    KEFIR_FREE(mem, decl);
    return KEFIR_OK;
}

kefir_result_t kefir_ast_declarator_is_abstract(struct kefir_ast_declarator *decl, kefir_bool_t *result) {
    REQUIRE(decl != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST declarator"));
    REQUIRE(result != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid pointer to boolean"));

    switch (decl->klass) {
        case KEFIR_AST_DECLARATOR_IDENTIFIER:
            *result = decl->identifier != NULL;
            break;

        case KEFIR_AST_DECLARATOR_POINTER:
            REQUIRE_OK(kefir_ast_declarator_is_abstract(decl->pointer.declarator, result));
            break;

        case KEFIR_AST_DECLARATOR_ARRAY:
            REQUIRE_OK(kefir_ast_declarator_is_abstract(decl->array.declarator, result));
            break;

        case KEFIR_AST_DECLARATOR_FUNCTION:
            REQUIRE_OK(kefir_ast_declarator_is_abstract(decl->function.declarator, result));
            break;
    }
    return KEFIR_OK;
}