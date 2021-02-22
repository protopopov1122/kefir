#ifndef KEFIR_IR_PLATFORM_H_
#define KEFIR_IR_PLATFORM_H_

#include "kefir/core/basic-types.h"
#include "kefir/core/mem.h"
#include "kefir/ir/type.h"

typedef struct kefir_ir_target_type_info {
    kefir_size_t size;
    kefir_size_t alignment;
    kefir_bool_t aligned;
} kefir_ir_target_type_info_t;

typedef struct kefir_ir_target_platform {
    kefir_result_t (*type_info)(struct kefir_mem *,
                              struct kefir_ir_target_platform *,
                              const struct kefir_ir_type *,
                              struct kefir_ir_target_type_info *);
    kefir_result_t (*free)(struct kefir_mem *,
                         struct kefir_ir_target_platform *);

    void *payload;
} kefir_ir_target_platform_t;

#define KEFIR_IR_TARGET_PLATFORM_TYPE_INFO(platform, mem, type, info) \
    ((platform)->type_info((mem), (platform), (type), (info)))
#define KEFIR_IR_TARGET_PLATFORM_FREE(platform, mem) \
    ((platform)->free((mem), (platform)))

#endif