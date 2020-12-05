#include <stdio.h>
#include <stdarg.h>
#include "kefir/core/util.h"

const char *kefir_format(char *buf, kefir_size_t length, const char *format, ...) {
    va_list(args);
    va_start(args, format);
    vsnprintf(buf, length, format, args);
    va_end(args);
    return buf;
}