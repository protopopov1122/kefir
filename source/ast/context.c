#include "kefir/ast/context.h"
#include "kefir/core/error.h"
#include "kefir/core/util.h"

kefir_result_t kefir_ast_context_configuration_defaults(struct kefir_ast_context_configuration *config) {
    REQUIRE(config != NULL, KEFIR_SET_ERROR(KEFIR_INVALID_PARAMETER, "Expected valid AST context configuration"));

    *config = (struct kefir_ast_context_configuration){.analysis = {.permissive_pointer_conv = false,
                                                                    .non_strict_qualifiers = false,
                                                                    .fixed_enum_type = false,
                                                                    .implicit_function_declaration = false,
                                                                    .int_to_pointer = false}};
    return KEFIR_OK;
}
