#include "kefir/codegen/util.h"

kefir_size_t kefir_codegen_pad_aligned(kefir_size_t offset, kefir_size_t alignment) {
    const kefir_size_t padding = offset % alignment;
    if (padding != 0) {
        offset += alignment - padding;
    }
    return offset;
}