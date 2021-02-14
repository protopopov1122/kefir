#ifndef KEFIR_AST_SCOPE_H_
#define KEFIR_AST_SCOPE_H_

#include "kefir/core/mem.h"
#include "kefir/ast/type.h"
#include "kefir/ast/constants.h"
#include "kefir/core/hashtree.h"
#include "kefir/core/tree.h"

typedef enum kefir_ast_scoped_identifier_class {
    KEFIR_AST_SCOPE_IDENTIFIER_OBJECT,
    KEFIR_AST_SCOPE_IDENTIFIER_FUNCTION,
    KEFIR_AST_SCOPE_IDENTIFIER_TYPE_TAG,
    KEFIR_AST_SCOPE_IDENTIFIER_TYPE_DEFINITION,
    KEFIR_AST_SCOPE_IDENTIFIER_LABEL
} kefir_ast_scoped_identifier_class_t;

typedef enum kefir_ast_scoped_identifier_linkage {
    KEFIR_AST_SCOPED_IDENTIFIER_EXTERNAL_LINKAGE,
    KEFIR_AST_SCOPED_IDENTIFIER_INTERNAL_LINKAGE,
    KEFIR_AST_SCOPED_IDENTIFIER_NONE_LINKAGE
} kefir_ast_scoped_identifier_linkage_t;

typedef enum kefir_ast_scoped_identifier_storage {
    KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN,
    KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC,
    KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_THREAD_LOCAL,
    KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_EXTERN_THREAD_LOCAL,
    KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_STATIC_THREAD_LOCAL,
    KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_AUTO,
    KEFIR_AST_SCOPE_IDENTIFIER_STORAGE_REGISTER
} kefir_ast_scoped_identifier_storage_t;

#define KEFIR_AST_SCOPED_IDENTIFIER_PAYLOAD_SIZE (sizeof(kefir_uptr_t) * 4)

typedef struct kefir_ast_scoped_identifier {
    kefir_ast_scoped_identifier_class_t klass;
    union {
        struct {
            const struct kefir_ast_type *type;
            kefir_ast_scoped_identifier_storage_t storage;
        } object;
        const struct kefir_ast_type *type;
    };
    struct {
        unsigned char content[KEFIR_AST_SCOPED_IDENTIFIER_PAYLOAD_SIZE];
        void *ptr;
    } payload;
} kefir_ast_scoped_identifier_t;

typedef struct kefir_ast_identifier_flat_scope_iterator {
    struct kefir_hashtree_node_iterator iter;

    const char *identifier;
    const struct kefir_ast_scoped_identifier *value;
} kefir_ast_identifier_flat_scope_iterator_t;

typedef struct kefir_ast_identifier_flat_scope {
    struct kefir_hashtree content;
} kefir_ast_identifier_flat_scope_t;

kefir_result_t kefir_ast_identifier_flat_scope_init(struct kefir_ast_identifier_flat_scope *);
kefir_result_t kefir_ast_identifier_flat_scope_free(struct kefir_mem *,
                                           struct kefir_ast_identifier_flat_scope *);
kefir_result_t kefir_ast_identifier_flat_scope_insert(struct kefir_mem *,
                                             struct kefir_ast_identifier_flat_scope *,
                                             const char *,
                                             const struct kefir_ast_scoped_identifier *);
kefir_result_t kefir_ast_identifier_flat_scope_at(const struct kefir_ast_identifier_flat_scope *,
                                         const char *,
                                         const struct kefir_ast_scoped_identifier **);
kefir_result_t kefir_ast_identifier_flat_scope_iter(const struct kefir_ast_identifier_flat_scope *,
                                           struct kefir_ast_identifier_flat_scope_iterator *);
kefir_result_t kefir_ast_identifier_flat_scope_next(const struct kefir_ast_identifier_flat_scope *,
                                           struct kefir_ast_identifier_flat_scope_iterator *);

typedef struct kefir_ast_identifier_block_scope {
    struct kefir_tree_node root;
    struct kefir_tree_node *top_scope;
} kefir_ast_identifier_block_scope_t;

kefir_result_t kefir_ast_identifier_block_scope_init(struct kefir_mem *,
                                      struct kefir_ast_identifier_block_scope *);
kefir_result_t kefir_ast_identifier_block_scope_free(struct kefir_mem *,
                                      struct kefir_ast_identifier_block_scope *);
struct kefir_ast_identifier_flat_scope *kefir_ast_identifier_block_scope_top(const struct kefir_ast_identifier_block_scope *);
kefir_result_t kefir_ast_identifier_block_scope_push(struct kefir_mem *,
                                      struct kefir_ast_identifier_block_scope *);
kefir_result_t kefir_ast_identifier_block_scope_pop(struct kefir_ast_identifier_block_scope *);

kefir_result_t kefir_ast_identifier_block_scope_insert(struct kefir_mem *,
                                        const struct kefir_ast_identifier_block_scope *,
                                        const char *,
                                        const struct kefir_ast_scoped_identifier *);
kefir_result_t kefir_ast_identifier_block_scope_at(const struct kefir_ast_identifier_block_scope *,
                                    const char *,
                                    const struct kefir_ast_scoped_identifier **);


#endif