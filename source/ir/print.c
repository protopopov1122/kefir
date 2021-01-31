#include "kefir/ir/print.h"
#include "kefir/core/hashtree.h"
#include "kefir/ir/builtins.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"
#include "kefir/ir/symbolic.h"

static kefir_result_t kefir_ir_print_type(FILE *fp, const char *prefix, struct kefir_ir_type *type) {
    for (kefir_size_t i = 0; i < kefir_ir_type_raw_length(type); i++) {
        struct kefir_ir_typeentry *typeentry = kefir_ir_type_at(type, i);
        switch (typeentry->typecode) {
            case KEFIR_IR_TYPE_PAD:
                fprintf(fp, "%spad(" KEFIR_INT64_FMT ")", prefix, typeentry->param);
                break;

            case KEFIR_IR_TYPE_STRUCT:
                fprintf(fp, "%sstruct(" KEFIR_INT64_FMT ")", prefix, typeentry->param);
                break;

            case KEFIR_IR_TYPE_ARRAY:
                fprintf(fp, "%sstruct(" KEFIR_INT64_FMT ")", prefix, typeentry->param);
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

static kefir_result_t kefir_ir_print_function_declaration(FILE *fp, struct kefir_ir_function_decl *decl) {
    fprintf(fp, "declare %s", decl->identifier);
    if (decl->alias != NULL) {
        fprintf(fp, " as %s", decl->alias);
    }
    fprintf(fp," = {\n\tparams:\n");
    REQUIRE_OK(kefir_ir_print_type(fp, "\t\t", decl->params));
    if (decl->vararg) {
        fprintf(fp, "\tvararg: yes\n");
    } else {
        fprintf(fp, "\tvararg: no\n");
    }
    fprintf(fp, "\treturns:\n");
    REQUIRE_OK(kefir_ir_print_type(fp, "\t\t", decl->result));
    fprintf(fp, "}\n");
    return KEFIR_OK;
}

static kefir_result_t kefir_ir_print_function(FILE *fp, const struct kefir_ir_function *func)  {
    fprintf(fp, "implement %s = {\n", func->declaration->identifier);
    if (func->locals != NULL) {
        fprintf(fp, "\tlocals:\n");
        REQUIRE_OK(kefir_ir_print_type(fp, "\t\t", func->locals));
    }
    fprintf(fp, "\tbody:\n");
    for (kefir_size_t i = 0; i < kefir_irblock_length(&func->body); i++) {
        const struct kefir_irinstr *instr = kefir_irblock_at(&func->body, i);
        fprintf(fp, "\t\t%s\t" KEFIR_INT64_FMT "\n", kefir_iropcode_symbol(instr->opcode), instr->arg.i64);
    }
    fprintf(fp, "}\n");
    return KEFIR_OK;
}

kefir_result_t kefir_ir_print_module(FILE *fp, const struct kefir_ir_module *module) {
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
        REQUIRE_OK(kefir_ir_print_function_declaration(fp, (struct kefir_ir_function_decl *) node->value));
    }
    fprintf(fp, "\n");
    for (const struct kefir_ir_function *func = kefir_ir_module_function_iter(module, &iter);
        func != NULL;
        func = kefir_ir_module_function_next(&iter)) {
        REQUIRE_OK(kefir_ir_print_function(fp, func));
    }
    return KEFIR_OK;
}