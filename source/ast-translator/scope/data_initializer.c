#include "kefir/ast-translator/scope/translator.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/ir/type_tree.h"
#include "kefir/ast/constant_expression.h"

static kefir_result_t translate_scalar(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                       struct kefir_ast_type_layout *type_layout, const struct kefir_ir_type *type,
                                       struct kefir_ast_initializer *initializer, struct kefir_ir_data *data,
                                       kefir_size_t base_slot) {
    struct kefir_ast_node_base *expr = kefir_ast_initializer_head(initializer);
    if (expr != NULL) {
        struct kefir_ast_constant_expression_value value;
        REQUIRE_OK(kefir_ast_constant_expression_value_evaluate(mem, context, expr, &value));
        switch (value.klass) {
            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_NONE:
                return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Unexpected constant expression value type");

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_INTEGER:
                REQUIRE_OK(kefir_ir_data_set_integer(data, base_slot, value.integer));
                break;

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_FLOAT:
                if (kefir_ir_type_at(type, type_layout->value)->typecode == KEFIR_IR_TYPE_FLOAT32) {
                    REQUIRE_OK(kefir_ir_data_set_float32(data, base_slot, value.floating_point));
                } else {
                    REQUIRE_OK(kefir_ir_data_set_float64(data, base_slot, value.floating_point));
                }
                break;

            case KEFIR_AST_CONSTANT_EXPRESSION_CLASS_ADDRESS:
                switch (value.pointer.type) {
                    case KEFIR_AST_CONSTANT_EXPRESSION_POINTER_IDENTIFER:
                        REQUIRE_OK(kefir_ir_data_set_pointer(data, base_slot, value.pointer.base.literal,
                                                             value.pointer.offset));
                        break;

                    case KEFIR_AST_CONSTANT_EXPRESSION_POINTER_INTEGER:
                        REQUIRE_OK(kefir_ir_data_set_integer(data, base_slot,
                                                             value.pointer.base.integral + value.pointer.offset));
                        break;

                    case KEFIR_AST_CONSTANT_EXPRESSION_POINTER_LITERAL:
                        return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED,
                                               "String literal initializers are not supported yet");
                }
                break;
        }
    } else {
        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Scalar initializer list cannot be empty");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ast_translate_data_initializer(struct kefir_mem *mem, const struct kefir_ast_context *context,
                                                    struct kefir_ast_type_layout *type_layout,
                                                    const struct kefir_ir_type *type,
                                                    struct kefir_ast_initializer *initializer,
                                                    struct kefir_ir_data *data, kefir_size_t base_slot) {
    REQUIRE(mem != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid memory allocator"));
    REQUIRE(context != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST context"));
    REQUIRE(type_layout != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST type layout"));
    REQUIRE(type != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR type"));
    REQUIRE(initializer != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid AST initializer"));
    REQUIRE(data != NULL, KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid IR data"));

    const struct kefir_ast_type *ast_type = kefir_ast_unqualified_type(type_layout->type);

    if (KEFIR_AST_TYPE_IS_SCALAR_TYPE(ast_type)) {
        REQUIRE_OK(translate_scalar(mem, context, type_layout, type, initializer, data, base_slot));
    } else {
        return KEFIR_SET_ERROR(KEFIR_NOT_SUPPORTED, "Non-scalar globals are currently not supported");
    }
    return KEFIR_OK;
}
