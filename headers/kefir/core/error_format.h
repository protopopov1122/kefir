#ifndef KEFIR_CORE_ERROR_FORMAT_H_
#define KEFIR_CORE_ERROR_FORMAT_H_

#include "kefir/core/error.h"
#include <stdio.h>

void kefir_format_error_tabular(FILE *, const struct kefir_error *);
void kefir_format_error_json(FILE *, const struct kefir_error *);

#endif
