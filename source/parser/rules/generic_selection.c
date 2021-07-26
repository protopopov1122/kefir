#include "kefir/parser/rule_helpers.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

DEFINE_MATCH(GENERIC_SELECTION) {
    // TODO Implement generic selection parser
    return KEFIR_SET_ERROR(KEFIR_NO_MATCH, "Generic selection parsing is not supported yet");
}
END_PROCEDURE

DEFINE_REDUCE(GENERIC_SELECTION) {}
END_PROCEDURE

DEFINE_RULE(GENERIC_SELECTION);
