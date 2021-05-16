#ifndef KEFIR_AST_DESGINATOR_H_
#define KEFIR_AST_DESGINATOR_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/core/symbol_table.h"

typedef enum kefir_ast_designator_type {
    KEFIR_AST_DESIGNATOR_MEMBER,
    KEFIR_AST_DESIGNATOR_SUBSCRIPT
} kefir_ast_designator_type_t;

typedef struct kefir_ast_designator {
    kefir_ast_designator_type_t type;
    union {
        const char *member;
        kefir_size_t index;
    };
    struct kefir_ast_designator *next;
} kefir_ast_designator_t;

struct kefir_ast_designator *kefir_ast_new_member_desginator(struct kefir_mem *,
                                                         struct kefir_symbol_table *,
                                                         const char *,
                                                         struct kefir_ast_designator *);

struct kefir_ast_designator *kefir_ast_new_index_desginator(struct kefir_mem *,
                                                        kefir_size_t,
                                                        struct kefir_ast_designator *);

kefir_result_t kefir_ast_designator_free(struct kefir_mem *,
                                     struct kefir_ast_designator *);
                        
struct kefir_ast_designator *kefir_ast_designator_clone(struct kefir_mem *,
                                                    const struct kefir_ast_designator *);

#endif
