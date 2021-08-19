#include "kefir/util/char32.h"
#include "kefir/core/util.h"

kefir_size_t kefir_strlen32(const kefir_char32_t *string) {
    REQUIRE(string != NULL, 0);
    kefir_size_t length = 0;
    while (string[length] != U'\0') {
        length++;
    }
    return length;
}
