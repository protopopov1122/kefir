#ifndef KEFIR_IR_PLATFORM_H_
#define KEFIR_IR_PLATFORM_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/ir/type.h"

typedef void *kefir_ir_target_platform_opaque_type_t;

typedef struct kefir_ir_target_type_info {
    kefir_size_t size;
    kefir_size_t alignment;
    kefir_bool_t aligned;
    kefir_size_t relative_offset;
} kefir_ir_target_type_info_t;

typedef struct kefir_ir_target_platform {
    kefir_result_t (*get_type)(struct kefir_mem *,
                              struct kefir_ir_target_platform *,
                              const struct kefir_ir_type *,
                              kefir_ir_target_platform_opaque_type_t *);
    kefir_result_t (*free_type)(struct kefir_mem *,
                              struct kefir_ir_target_platform *,
                              kefir_ir_target_platform_opaque_type_t);
    kefir_result_t (*type_info)(struct kefir_mem *,
                              struct kefir_ir_target_platform *,
                              kefir_ir_target_platform_opaque_type_t,
                              kefir_size_t,
                              struct kefir_ir_target_type_info *);
    kefir_result_t (*free)(struct kefir_mem *,
                         struct kefir_ir_target_platform *);

    void *payload;
} kefir_ir_target_platform_t;

#define KEFIR_IR_TARGET_PLATFORM_GET_TYPE(mem, platform, ir_type, type_ptr) \
    ((platform)->get_type((mem), (platform), (ir_type), (type_ptr)))
#define KEFIR_IR_TARGET_PLATFORM_FREE_TYPE(mem, platform, type_ptr) \
    ((platform)->free_type((mem), (platform), (type_ptr)))
#define KEFIR_IR_TARGET_PLATFORM_TYPE_INFO(mem, platform, type, index, info) \
    ((platform)->type_info((mem), (platform), (type), (index), (info)))
#define KEFIR_IR_TARGET_PLATFORM_FREE(mem, platform) \
    ((platform)->free((mem), (platform)))

#endif