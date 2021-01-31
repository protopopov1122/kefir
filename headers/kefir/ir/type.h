#ifndef KEFIR_IR_TYPE_H_
#define KEFIR_IR_TYPE_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/vector.h"
#include "kefir/core/mem.h"

typedef enum kefir_ir_typecode {
    // Padding
    KEFIR_IR_TYPE_PAD = 0x0,
    // Aggregate types
    KEFIR_IR_TYPE_STRUCT,
    KEFIR_IR_TYPE_ARRAY,
    KEFIR_IR_TYPE_UNION,
    KEFIR_IR_TYPE_MEMORY,
    // Fixed scalars
    KEFIR_IR_TYPE_INT8,
    KEFIR_IR_TYPE_INT16,
    KEFIR_IR_TYPE_INT32,
    KEFIR_IR_TYPE_INT64,
    KEFIR_IR_TYPE_FLOAT32,
    KEFIR_IR_TYPE_FLOAT64,
    // Platform-dependent scalars
    KEFIR_IR_TYPE_BOOL,
    KEFIR_IR_TYPE_CHAR,
    KEFIR_IR_TYPE_SHORT,
    KEFIR_IR_TYPE_INT,
    KEFIR_IR_TYPE_LONG,
    KEFIR_IR_TYPE_WORD,
    // Built-ins
    KEFIR_IR_TYPE_BUILTIN,
    // > 64-bit scalars are not supported yet
    KEFIR_IR_TYPE_COUNT, // Auxilary
} kefir_ir_typecode_t;

typedef struct kefir_ir_typeentry {
    kefir_ir_typecode_t typecode;
    kefir_uint32_t alignment : 8;
    kefir_int64_t param;
} kefir_ir_typeentry_t;

typedef struct kefir_ir_type {
    struct kefir_vector vector;
} kefir_ir_type_t;

kefir_result_t kefir_ir_type_init(struct kefir_ir_type *, void *, kefir_size_t);
kefir_size_t kefir_ir_type_raw_available(const struct kefir_ir_type *);
kefir_size_t kefir_ir_type_raw_length(const struct kefir_ir_type *);
struct kefir_ir_typeentry *kefir_ir_type_raw_at(const struct kefir_ir_type *, kefir_size_t);
kefir_result_t kefir_ir_type_appendi64(struct kefir_ir_type *, const struct kefir_ir_typeentry *);
kefir_result_t kefir_ir_type_append_v(struct kefir_ir_type *, kefir_ir_typecode_t, kefir_uint32_t, kefir_int64_t);
kefir_result_t kefir_ir_type_append_e(struct kefir_ir_type *, const struct kefir_ir_type *, kefir_size_t);
kefir_result_t kefir_ir_type_alloc(struct kefir_mem *, kefir_size_t, struct kefir_ir_type *);
kefir_result_t kefir_ir_type_realloc(struct kefir_mem *, kefir_size_t, struct kefir_ir_type *);
kefir_result_t kefir_ir_type_free(struct kefir_mem *, struct kefir_ir_type *);

struct kefir_ir_typeentry *kefir_ir_type_at(const struct kefir_ir_type *, kefir_size_t);
kefir_size_t kefir_ir_type_total_length(const struct kefir_ir_type *);
kefir_size_t kefir_ir_type_nodes(const struct kefir_ir_type *);
kefir_size_t kefir_ir_type_subnodes(const struct kefir_ir_type *, kefir_size_t);
kefir_size_t kefir_ir_type_node_total_length(const struct kefir_ir_type *, kefir_size_t);
kefir_size_t kefir_ir_type_node_slots(const struct kefir_ir_type *, kefir_size_t);
kefir_size_t kefir_ir_type_total_slots(const struct kefir_ir_type *);

typedef kefir_result_t (*kefir_ir_type_visitor_callback_t)(const struct kefir_ir_type *,
                                                       kefir_size_t,
                                                       const struct kefir_ir_typeentry *,
                                                       void *);

typedef kefir_ir_type_visitor_callback_t kefir_ir_type_visitor_hook_t;

typedef struct kefir_ir_type_visitor {
    kefir_ir_type_visitor_callback_t visit[KEFIR_IR_TYPE_COUNT];
    kefir_ir_type_visitor_hook_t prehook;
    kefir_ir_type_visitor_hook_t posthook;
} kefir_ir_type_visitor_t;

kefir_result_t kefir_ir_type_visitor_init(struct kefir_ir_type_visitor *, kefir_ir_type_visitor_callback_t);
kefir_result_t kefir_ir_type_visitor_list_nodes(const struct kefir_ir_type *,
                                          const struct kefir_ir_type_visitor *,
                                          void *,
                                          kefir_size_t,
                                          kefir_size_t);

#define KEFIR_IR_TYPE_VISITOR_INIT_FIXED_INTEGERS(visitor, callback) \
    do { \
        (visitor)->visit[KEFIR_IR_TYPE_INT8] = (callback); \
        (visitor)->visit[KEFIR_IR_TYPE_INT16] = (callback); \
        (visitor)->visit[KEFIR_IR_TYPE_INT32] = (callback); \
        (visitor)->visit[KEFIR_IR_TYPE_INT64] = (callback); \
    } while (0)
#define KEFIR_IR_TYPE_VISITOR_INIT_ALIASED_INTEGERS(visitor, callback) \
    do { \
        (visitor)->visit[KEFIR_IR_TYPE_BOOL] = (callback); \
        (visitor)->visit[KEFIR_IR_TYPE_CHAR] = (callback); \
        (visitor)->visit[KEFIR_IR_TYPE_SHORT] = (callback); \
        (visitor)->visit[KEFIR_IR_TYPE_INT] = (callback); \
        (visitor)->visit[KEFIR_IR_TYPE_LONG] = (callback); \
        (visitor)->visit[KEFIR_IR_TYPE_WORD] = (callback); \
    } while (0)
#define KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS(visitor, callback) \
    do { \
        KEFIR_IR_TYPE_VISITOR_INIT_FIXED_INTEGERS((visitor), (callback)); \
        KEFIR_IR_TYPE_VISITOR_INIT_ALIASED_INTEGERS((visitor), (callback)); \
    } while (0)
#define KEFIR_IR_TYPE_VISITOR_INIT_FIXED_FP(visitor, callback) \
    do { \
        (visitor)->visit[KEFIR_IR_TYPE_FLOAT32] = (callback); \
        (visitor)->visit[KEFIR_IR_TYPE_FLOAT64] = (callback); \
    } while (0)
#define KEFIR_IR_TYPE_VISITOR_INIT_SCALARS(visitor, callback) \
    do { \
        KEFIR_IR_TYPE_VISITOR_INIT_INTEGERS((visitor), (callback)); \
        KEFIR_IR_TYPE_VISITOR_INIT_FIXED_FP((visitor), (callback)); \
    } while (0)

typedef struct kefir_ir_type_iterator {
    const struct kefir_ir_type *type;
    kefir_size_t index;
    kefir_size_t slot;
} kefir_ir_type_iterator_t;

kefir_result_t kefir_ir_type_iterator_init(const struct kefir_ir_type *, struct kefir_ir_type_iterator *);
kefir_result_t kefir_ir_type_iterator_goto(struct kefir_ir_type_iterator *, kefir_size_t);
kefir_result_t kefir_ir_type_iterator_goto_field(struct kefir_ir_type_iterator *, kefir_size_t);
kefir_result_t kefir_ir_type_iterator_goto_index(struct kefir_ir_type_iterator *, kefir_size_t);

#endif