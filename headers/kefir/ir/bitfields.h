#ifndef KEFIR_IR_BITFIELDS_H_
#define KEFIR_IR_BITFIELDS_H_

#include "kefir/core/basic-types.h"
#include "kefir/ir/type.h"

typedef struct kefir_ir_bitfield {
    kefir_uint8_t offset;
    kefir_uint8_t width;
} kefir_ir_bitfield_t;

typedef struct kefir_ir_bitfield_allocator {
    const struct kefir_ir_bitfield *(*get_state)(struct kefir_ir_bitfield_allocator *);
    kefir_result_t (*reset)(struct kefir_ir_bitfield_allocator *);
    kefir_result_t (*init)(struct kefir_ir_bitfield_allocator *,
                          kefir_ir_typecode_t);
    kefir_result_t (*next)(struct kefir_ir_bitfield_allocator *,
                         kefir_uint8_t,
                         struct kefir_ir_bitfield *);
    kefir_result_t (*free)(struct kefir_mem *,
                         struct kefir_ir_bitfield_allocator *);

    kefir_result_t (*update_colocated)(struct kefir_ir_bitfield_allocator *,
                                     struct kefir_ir_typeentry *,
                                     const struct kefir_ir_typeentry *);
    void *payload;
} kefir_ir_bitfield_allocator_t;

#define KEFIR_IR_BITFIELD_ALLOCATOR_GET_STATE(allocator) \
    ((allocator)->get_state((allocator)))
#define KEFIR_IR_BITFIELD_ALLOCATOR_RESET(allocator) \
    ((allocator)->reset((allocator)))
#define KEFIR_IR_BITFIELD_ALLOCATOR_INIT(allocator, typecode) \
    ((allocator)->init((allocator), (typecode)))
#define KEFIR_IR_BITFIELD_ALLOCATOR_NEXT(allocator, width, bitfield) \
    ((allocator)->next((allocator), (width), (bitfield)))
#define KEFIR_IR_BITFIELD_ALLOCATOR_FREE(mem, allocator) \
    ((allocator)->free((mem), (allocator)))
#define KEFIR_IR_BITFIELD_ALLOCATOR_UPDATE_COLOCATED(allocated, typeentry, colocated) \
    ((allocated)->update_colocated((allocated), (typeentry), (colocated)))

#endif