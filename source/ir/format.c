#include "kefir/ir/format.h"
#include "kefir/core/hashtree.h"
#include "kefir/ir/builtins.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/ir/mnemonic.h"
#include "kefir/ir/format_impl.h"

kefir_result_t kefir_ir_format_instr_none(FILE *fp, const struct kefir_ir_module *module, const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(fp != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid FILE pointer"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));
    fprintf(fp, "%s", kefir_iropcode_mnemonic(instr->opcode));
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_i64(FILE *fp, const struct kefir_ir_module *module, const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(fp != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid FILE pointer"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));
    fprintf(fp, "%s\t" KEFIR_INT64_FMT, kefir_iropcode_mnemonic(instr->opcode), instr->arg.i64);
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_u64(FILE *fp, const struct kefir_ir_module *module, const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(fp != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid FILE pointer"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));
    fprintf(fp, "%s\t" KEFIR_UINT64_FMT, kefir_iropcode_mnemonic(instr->opcode), instr->arg.u64);
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_f32(FILE *fp, const struct kefir_ir_module *module, const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(fp != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid FILE pointer"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));
    fprintf(fp, "%s\t%a", kefir_iropcode_mnemonic(instr->opcode), instr->arg.f32[0]);
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_f64(FILE *fp, const struct kefir_ir_module *module, const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(fp != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid FILE pointer"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));
    fprintf(fp, "%s\t%a", kefir_iropcode_mnemonic(instr->opcode), instr->arg.f64);
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_typeref(FILE *fp, const struct kefir_ir_module *module, const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(fp != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid FILE pointer"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));
    fprintf(fp, "%s\ttype(" KEFIR_UINT32_FMT ")[" KEFIR_UINT32_FMT "]",
        kefir_iropcode_mnemonic(instr->opcode), instr->arg.u32[0], instr->arg.u32[1]);
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_dataref(FILE *fp, const struct kefir_ir_module *module, const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(fp != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid FILE pointer"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));
    fprintf(fp, "%s\tdata(" KEFIR_UINT64_FMT ")",
        kefir_iropcode_mnemonic(instr->opcode), instr->arg.u64);
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_funcref(FILE *fp, const struct kefir_ir_module *module, const struct kefir_irinstr *instr) {
    REQUIRE(fp != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid FILE pointer"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR module"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));
    const char *symbol = kefir_ir_module_get_named_symbol(module, (kefir_id_t) instr->arg.u64);
    fprintf(fp, "%s\tfunction(%s)",
        kefir_iropcode_mnemonic(instr->opcode), symbol);
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_instr_coderef(FILE *fp, const struct kefir_ir_module *module, const struct kefir_irinstr *instr) {
    UNUSED(module);
    REQUIRE(fp != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid FILE pointer"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));
    fprintf(fp, "%s\toffset(" KEFIR_UINT64_FMT ")",
        kefir_iropcode_mnemonic(instr->opcode), instr->arg.u64);
    return KEFIR_OK;
}

static kefir_result_t kefir_ir_format_type_impl(FILE *fp, const char *prefix, struct kefir_ir_type *type) {
    for (kefir_size_t i = 0; i < kefir_ir_type_total_length(type); i++) {
        struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(type, i);
        switch (typeentry->typecode) {
            case KEFIR_IR_TYPE_PAD:
                fprintf(fp, "%spad(" KEFIR_INT64_FMT ")", prefix, typeentry->param);
                break;

            case KEFIR_IR_TYPE_STRUCT:
                fprintf(fp, "%sstruct(" KEFIR_INT64_FMT ")", prefix, typeentry->param);
                break;

            case KEFIR_IR_TYPE_ARRAY:
                fprintf(fp, "%sarray(" KEFIR_INT64_FMT ")", prefix, typeentry->param);
                break;

            case KEFIR_IR_TYPE_UNION:
                fprintf(fp, "%sunion(" KEFIR_INT64_FMT ")", prefix, typeentry->param);
                break;

            case KEFIR_IR_TYPE_MEMORY:
                fprintf(fp, "%smemory(" KEFIR_INT64_FMT ")", prefix, typeentry->param);
                break;

            case KEFIR_IR_TYPE_INT8:
                fprintf(fp, "%sint8", prefix);
                break;

            case KEFIR_IR_TYPE_INT16:
                fprintf(fp, "%sint16", prefix);
                break;

            case KEFIR_IR_TYPE_INT32:
                fprintf(fp, "%sint32", prefix);
                break;

            case KEFIR_IR_TYPE_INT64:
                fprintf(fp, "%sint64", prefix);
                break;

            case KEFIR_IR_TYPE_BOOL:
                fprintf(fp, "%sbool", prefix);
                break;

            case KEFIR_IR_TYPE_CHAR:
                fprintf(fp, "%schar", prefix);
                break;

            case KEFIR_IR_TYPE_SHORT:
                fprintf(fp, "%sshort", prefix);
                break;

            case KEFIR_IR_TYPE_INT:
                fprintf(fp, "%sint", prefix);
                break;

            case KEFIR_IR_TYPE_LONG:
                fprintf(fp, "%slong", prefix);
                break;

            case KEFIR_IR_TYPE_FLOAT32:
                fprintf(fp, "%sfloat", prefix);
                break;

            case KEFIR_IR_TYPE_FLOAT64:
                fprintf(fp, "%sdouble", prefix);
                break;

            case KEFIR_IR_TYPE_WORD:
                fprintf(fp, "%sword", prefix);
                break;

            case KEFIR_IR_TYPE_BUILTIN:
                fprintf(fp, "%s", prefix);
                switch (typeentry->param) {
                    case KEFIR_IR_TYPE_BUILTIN_VARARG:
                        fprintf(fp, "vararg");
                        break;
                    
                    default:
                        return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid built-in type code");
                }
                break;

            default:
                return KEFIR_SET_ERROR(KEFIR_MALFORMED_ARG, "Expected valid type code");
        }
        if (typeentry->alignment > 0) {
            fprintf(fp, " align as " KEFIR_UINT32_FMT, typeentry->alignment);
        }
        fprintf(fp, "\n");
    }
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_type(FILE *fp, struct kefir_ir_type *type) {
    return kefir_ir_format_type_impl(fp, "", type);
}

static kefir_result_t kefir_ir_format_function_declaration(FILE *fp, struct kefir_ir_function_decl *decl) {
    fprintf(fp, "declare %s", decl->identifier);
    if (decl->alias != NULL) {
        fprintf(fp, " as %s", decl->alias);
    }
    fprintf(fp," = {\n\tparams:\n");
    REQUIRE_OK(kefir_ir_format_type_impl(fp, "\t\t", decl->params));
    if (decl->vararg) {
        fprintf(fp, "\tvararg: yes\n");
    } else {
        fprintf(fp, "\tvararg: no\n");
    }
    fprintf(fp, "\treturns:\n");
    REQUIRE_OK(kefir_ir_format_type_impl(fp, "\t\t", decl->result));
    fprintf(fp, "}\n");
    return KEFIR_OK;
}

static kefir_result_t kefir_ir_format_function(FILE *fp, const struct kefir_ir_module *module, const struct kefir_ir_function *func)  {
    fprintf(fp, "implement %s = {\n", func->declaration->identifier);
    if (func->locals != NULL) {
        fprintf(fp, "\tlocals:\n");
        REQUIRE_OK(kefir_ir_format_type_impl(fp, "\t\t", func->locals));
    }
    fprintf(fp, "\tbody:\n");
    for (kefir_size_t i = 0; i < kefir_irblock_length(&func->body); i++) {
        const struct kefir_irinstr *instr = kefir_irblock_at(&func->body, i);
        fprintf(fp, "\t\t");
        REQUIRE_OK(kefir_ir_format_instr(fp, module, instr));
        fprintf(fp, "\n");
    }
    fprintf(fp, "}\n");
    return KEFIR_OK;
}

kefir_result_t kefir_ir_format_module(FILE *fp, const struct kefir_ir_module *module) {
    struct kefir_hashtree_node_iterator iter;
    const struct kefir_list_entry *entry = NULL;
    for (const char *global = kefir_ir_module_globals_iter(module, &entry);
        global != NULL;
        global = kefir_ir_module_symbol_iter_next(&entry)) {
        fprintf(fp, "global %s\n", global);
    }
    fprintf(fp, "\n");
    for (const char *external = kefir_ir_module_externals_iter(module, &entry);
        external != NULL;
        external = kefir_ir_module_symbol_iter_next(&entry)) {
        fprintf(fp, "external %s\n", external);
    }
    fprintf(fp, "\n");
    for (const struct kefir_hashtree_node *node = kefir_hashtree_iter(&module->function_declarations, &iter);
        node != NULL;
        node = kefir_hashtree_next(&iter)) {
        REQUIRE_OK(kefir_ir_format_function_declaration(fp, (struct kefir_ir_function_decl *) node->value));
    }
    fprintf(fp, "\n");
    for (const struct kefir_ir_function *func = kefir_ir_module_function_iter(module, &iter);
        func != NULL;
        func = kefir_ir_module_function_next(&iter)) {
        REQUIRE_OK(kefir_ir_format_function(fp, module, func));
    }
    return KEFIR_OK;
}