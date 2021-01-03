#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/ir/module.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"
#include "kefir/codegen/amd64/system-v/abi.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    REQUIRE_OK(kefir_codegen_amd64_sysv_init(&codegen, stdout, mem));
    codegen.asmgen.settings.enable_comments = false;
    codegen.asmgen.settings.enable_identation = false;

    struct kefir_ir_type *int8type = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE_OK(kefir_ir_type_append_v(int8type, KEFIR_IR_TYPE_INT8, 0, 0));
    struct kefir_ir_data *int8data1 = kefir_ir_module_new_named_data(mem, &module, "int8_1", int8type);
    REQUIRE_OK(kefir_ir_data_set_i64(int8data1, 0, 104));
    kefir_ir_module_new_named_data(mem, &module, "int8_2", int8type);

    struct kefir_ir_type *int16type = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE_OK(kefir_ir_type_append_v(int16type, KEFIR_IR_TYPE_INT16, 0, 0));
    struct kefir_ir_data *int16data1 = kefir_ir_module_new_named_data(mem, &module, "int16_1", int16type);
    REQUIRE_OK(kefir_ir_data_set_i64(int16data1, 0, 2589));
    kefir_ir_module_new_named_data(mem, &module, "int16_2", int16type);

    struct kefir_ir_type *int32type = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE_OK(kefir_ir_type_append_v(int32type, KEFIR_IR_TYPE_INT32, 0, 0));
    struct kefir_ir_data *int32data1 = kefir_ir_module_new_named_data(mem, &module, "int32_1", int32type);
    REQUIRE_OK(kefir_ir_data_set_i64(int32data1, 0, 0x1ffff));
    kefir_ir_module_new_named_data(mem, &module, "int32_2", int32type);

    struct kefir_ir_type *int64type = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE_OK(kefir_ir_type_append_v(int64type, KEFIR_IR_TYPE_INT64, 0, 0));
    struct kefir_ir_data *int64data1 = kefir_ir_module_new_named_data(mem, &module, "int64_1", int64type);
    REQUIRE_OK(kefir_ir_data_set_i64(int64data1, 0, 0x1ffffffff));
    kefir_ir_module_new_named_data(mem, &module, "int64_2", int64type);

    struct kefir_ir_type *float32type = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE_OK(kefir_ir_type_append_v(float32type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    struct kefir_ir_data *float32data1 = kefir_ir_module_new_named_data(mem, &module, "float32_1", float32type);
    REQUIRE_OK(kefir_ir_data_set_f32(float32data1, 0, 3.14159));
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, "float32_1"));
    kefir_ir_module_new_named_data(mem, &module, "float32_2", float32type);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, "float32_2"));

    struct kefir_ir_type *float64type = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE_OK(kefir_ir_type_append_v(float64type, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    struct kefir_ir_data *float64data1 = kefir_ir_module_new_named_data(mem, &module, "float64_1", float64type);
    REQUIRE_OK(kefir_ir_data_set_f64(float64data1, 0, 2.718281828));
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, "float64_1"));
    kefir_ir_module_new_named_data(mem, &module, "float64_2", float64type);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, "float64_2"));

    REQUIRE_OK(KEFIR_CODEGEN_TRANSLATE(&codegen.iface, &module));
    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(&codegen.iface));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return KEFIR_OK;
}