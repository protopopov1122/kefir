#ifndef KEFIR_CORE_OPTIONAL_H_
#define KEFIR_CORE_OPTIONAL_H_

#include <string.h>
#include "kefir/core/basic-types.h"

#define KEFIR_OPTIONAL_TYPE(_type) \
    struct {                       \
        kefir_bool_t empty;        \
        _type value;               \
    }

#define KEFIR_NAMED_OPTIONAL_TYPE(_name, _type) \
    struct _name {                              \
        kefir_bool_t empty;                     \
        _type value;                            \
    }

#define KEFIR_OPTIONAL_EMPTY(_opt) ((_opt)->empty)

#define KEFIR_OPTIONAL_VALUE(_opt) ((_opt)->empty ? NULL : &(_opt)->value)

#define KEFIR_OPTIONAL_SET_EMPTY(_opt)                    \
    do {                                                  \
        (_opt)->empty = true;                             \
        memset(&(_opt)->value, 0, sizeof((_opt)->value)); \
    } while (0)

#define KEFIR_OPTIONAL_SET_VALUE(_opt, _value) \
    do {                                       \
        (_opt)->empty = false;                 \
        (_opt)->value = (_value);              \
    } while (0)

#endif
