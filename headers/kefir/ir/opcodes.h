#ifndef KEFIR_IR_OPCODES_H_
#define KEFIR_IR_OPCODES_H_

#include "kefir/core/base.h"

typedef enum kefir_iropcode {
    KEFIR_IROPCODE_RESERVED = 0x00,
    KEFIR_IROPCODE_NOP = 0x01,
    KEFIR_IROPCODE_RET = 0x02,

    KEFIR_IROPCODE_PUSH = 0x10,
    KEFIR_IROPCODE_POP = 0x11,

    KEFIR_IROPCODE_IADD = 0x20
} kefir_iropcode_t;

#endif