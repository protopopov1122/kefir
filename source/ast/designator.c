#include <string.h>
#include "kefir/ast/designator.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

struct kefir_ast_designator *kefir_ast_new_member_desginator(struct kefir_mem *mem,
                                                         struct kefir_symbol_table *symbols,
                                                         const char *member,
                                                         struct kefir_ast_designator *child) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(member != NULL && strlen(member) > 0, NULL);

    if (symbols != NULL) {
        member = kefir_symbol_table_insert(mem, symbols, member, NULL);
        REQUIRE(member != NULL, NULL);
    }
    struct kefir_ast_designator *designator = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_designator));
    REQUIRE(designator != NULL, NULL);

    designator->type = KEFIR_AST_DESIGNATOR_MEMBER;
    designator->member = member;
    designator->next = child;
    return designator;
}

struct kefir_ast_designator *kefir_ast_new_index_desginator(struct kefir_mem *mem,
                                                        kefir_size_t index,
                                                        struct kefir_ast_designator *child) {
    REQUIRE(mem != NULL, NULL);

    struct kefir_ast_designator *designator = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_designator));
    REQUIRE(designator != NULL, NULL);

    designator->type = KEFIR_AST_DESIGNATOR_SUBSCRIPT;
    designator->index = index;
    designator->next = child;
    return designator;
}

kefir_result_t kefir_ast_designator_free(struct kefir_mem *mem,
                                     struct kefir_ast_designator *designator) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(designator != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST designator"));

    if (designator->next != NULL) {
        REQUIRE_OK(kefir_ast_designator_free(mem, designator->next));
        designator->next = NULL;
    }
    designator->member = NULL;
    KEFIR_FREE(mem, designator);
    return KEFIR_OK;
}

struct kefir_ast_designator *kefir_ast_designator_clone(struct kefir_mem *mem,
                                                    const struct kefir_ast_designator *src) {
    REQUIRE(mem != NULL, NULL);
    REQUIRE(src != NULL, NULL);
    
    struct kefir_ast_designator *dst = KEFIR_MALLOC(mem, sizeof(struct kefir_ast_designator));
    REQUIRE(dst != NULL, NULL);
    dst->type = src->type;
    switch (src->type) {
        case KEFIR_AST_DESIGNATOR_MEMBER:
            dst->member = src->member;
            break;
        
        case KEFIR_AST_DESIGNATOR_SUBSCRIPT:
            dst->index = src->index;
            break;
    }
    dst->next = kefir_ast_designator_clone(mem, src->next);
    if (src->next != NULL) {
        REQUIRE_ELSE(dst->next != NULL, {
            KEFIR_FREE(mem, dst);
            return NULL;
        });
    }
    return dst;
}
