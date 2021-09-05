#ifndef KEFIR_CORE_ERROR_FORMAT_H_
#define KEFIR_CORE_ERROR_FORMAT_H_

#include "kefir/core/error.h"
#include <stdio.h>

void kefir_format_error(FILE *, const struct kefir_error *);

#endif
