#ifndef KEFIR_AST_SCOPE_H_
#define KEFIR_AST_SCOPE_H_

#include "kefir/core/mem.h"
#include "kefir/ast/type.h"
#include "kefir/ast/constants.h"
#include "kefir/ast/alignment.h"
#include "kefir/core/hashtree.h"
#include "kefir/core/tree.h"

#define KEFIR_AST_SCOPED_IDENTIFIER_PAYLOAD_SIZE (sizeof(kefir_uptr_t) * 4)

typedef struct kefir_ast_scoped_identifier {
    kefir_ast_scoped_identifier_class_t klass;
    union {
        struct {
            const struct kefir_ast_type *type;
            struct kefir_ast_alignment alignment;
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

    kefir_result_t (*remove_callback)(struct kefir_mem *, struct kefir_ast_scoped_identifier *, void *);
    void *remove_payload;
} kefir_ast_identifier_flat_scope_t;

kefir_result_t kefir_ast_identifier_flat_scope_init(struct kefir_ast_identifier_flat_scope *);
kefir_result_t kefir_ast_identifier_flat_scope_free(struct kefir_mem *,
                                                struct kefir_ast_identifier_flat_scope *);
kefir_result_t kefir_ast_identifier_flat_scope_on_removal(struct kefir_ast_identifier_flat_scope *,
                                                      kefir_result_t (*)(struct kefir_mem *, struct kefir_ast_scoped_identifier *, void *),
                                                      void *);
kefir_result_t kefir_ast_identifier_flat_scope_insert(struct kefir_mem *,
                                                  struct kefir_ast_identifier_flat_scope *,
                                                  const char *,
                                                  struct kefir_ast_scoped_identifier *);
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

    kefir_result_t (*remove_callback)(struct kefir_mem *, struct kefir_ast_scoped_identifier *, void *);
    void *remove_payload;
} kefir_ast_identifier_block_scope_t;

kefir_result_t kefir_ast_identifier_block_scope_init(struct kefir_mem *,
                                                 struct kefir_ast_identifier_block_scope *);
kefir_result_t kefir_ast_identifier_block_scope_free(struct kefir_mem *,
                                                 struct kefir_ast_identifier_block_scope *);
kefir_result_t kefir_ast_identifier_block_scope_on_removal(struct kefir_ast_identifier_block_scope *,
                                                       kefir_result_t (*)(struct kefir_mem *, struct kefir_ast_scoped_identifier *, void *),
                                                       void *);
struct kefir_ast_identifier_flat_scope *kefir_ast_identifier_block_scope_top(const struct kefir_ast_identifier_block_scope *);
kefir_result_t kefir_ast_identifier_block_scope_push(struct kefir_mem *,
                                                 struct kefir_ast_identifier_block_scope *);
kefir_result_t kefir_ast_identifier_block_scope_pop(struct kefir_ast_identifier_block_scope *);

kefir_result_t kefir_ast_identifier_block_scope_insert(struct kefir_mem *,
                                                   const struct kefir_ast_identifier_block_scope *,
                                                   const char *,
                                                   struct kefir_ast_scoped_identifier *);
kefir_result_t kefir_ast_identifier_block_scope_at(const struct kefir_ast_identifier_block_scope *,
                                               const char *,
                                               const struct kefir_ast_scoped_identifier **);


#endif