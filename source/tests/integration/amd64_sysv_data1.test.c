#include <stdlib.h>
#include <stdio.h>
#include "kefir/ir/function.h"
#include "kefir/ir/module.h"
#include "kefir/ir/builder.h"
#include "kefir/core/mem.h"
#include "kefir/core/util.h"
#include "kefir/codegen/amd64-sysv.h"
#include "kefir/codegen/amd64/system-v/abi.h"

kefir_result_t kefir_int_test(struct kefir_mem *mem) {
    struct kefir_codegen_amd64 codegen;
    struct kefir_ir_module module;
    REQUIRE_OK(kefir_ir_module_alloc(mem, &module));
    REQUIRE_OK(kefir_codegen_amd64_sysv_init(&codegen, stdout));
    codegen.asmgen.settings.enable_comments = false;
    codegen.asmgen.settings.enable_identation = false;

    struct kefir_ir_type *int8type = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, int8type, KEFIR_IR_TYPE_INT8, 0, 0));
    struct kefir_ir_data *int8data1 = kefir_ir_module_new_named_data(mem, &module, "int8_1", int8type);
    REQUIRE_OK(kefir_ir_data_set_i64(int8data1, 0, 104));
    kefir_ir_module_new_named_data(mem, &module, "int8_2", int8type);

    struct kefir_ir_type *int16type = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, int16type, KEFIR_IR_TYPE_INT16, 0, 0));
    struct kefir_ir_data *int16data1 = kefir_ir_module_new_named_data(mem, &module, "int16_1", int16type);
    REQUIRE_OK(kefir_ir_data_set_i64(int16data1, 0, 2589));
    kefir_ir_module_new_named_data(mem, &module, "int16_2", int16type);

    struct kefir_ir_type *int32type = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, int32type, KEFIR_IR_TYPE_INT32, 0, 0));
    struct kefir_ir_data *int32data1 = kefir_ir_module_new_named_data(mem, &module, "int32_1", int32type);
    REQUIRE_OK(kefir_ir_data_set_i64(int32data1, 0, 0x1ffff));
    kefir_ir_module_new_named_data(mem, &module, "int32_2", int32type);

    struct kefir_ir_type *int64type = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, int64type, KEFIR_IR_TYPE_INT64, 0, 0));
    struct kefir_ir_data *int64data1 = kefir_ir_module_new_named_data(mem, &module, "int64_1", int64type);
    REQUIRE_OK(kefir_ir_data_set_i64(int64data1, 0, 0x1ffffffff));
    kefir_ir_module_new_named_data(mem, &module, "int64_2", int64type);

    struct kefir_ir_type *float32type = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, float32type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    struct kefir_ir_data *float32data1 = kefir_ir_module_new_named_data(mem, &module, "float32_1", float32type);
    REQUIRE_OK(kefir_ir_data_set_f32(float32data1, 0, 3.14159));
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, "float32_1"));
    kefir_ir_module_new_named_data(mem, &module, "float32_2", float32type);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, "float32_2"));

    struct kefir_ir_type *float64type = kefir_ir_module_new_type(mem, &module, 1, NULL);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, float64type, KEFIR_IR_TYPE_FLOAT64, 0, 0));
    struct kefir_ir_data *float64data1 = kefir_ir_module_new_named_data(mem, &module, "float64_1", float64type);
    REQUIRE_OK(kefir_ir_data_set_f64(float64data1, 0, 2.718281828));
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, "float64_1"));
    kefir_ir_module_new_named_data(mem, &module, "float64_2", float64type);
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, "float64_2"));

    struct kefir_ir_type *struct1_type = kefir_ir_module_new_type(mem, &module, 5, NULL);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, struct1_type, KEFIR_IR_TYPE_STRUCT, 0, 4));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, struct1_type, KEFIR_IR_TYPE_INT8, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, struct1_type, KEFIR_IR_TYPE_INT64, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, struct1_type, KEFIR_IR_TYPE_INT16, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, struct1_type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    struct kefir_ir_data *struct1_data1 = kefir_ir_module_new_named_data(mem, &module, "struct1_1", struct1_type);
    REQUIRE_OK(kefir_ir_data_set_i64(struct1_data1, 1, 127));
    REQUIRE_OK(kefir_ir_data_set_i64(struct1_data1, 2, 0x2ffffffff));
    REQUIRE_OK(kefir_ir_data_set_i64(struct1_data1, 3, 4096));
    REQUIRE_OK(kefir_ir_data_set_f32(struct1_data1, 4, 106.9994));
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, "struct1_1"));

    struct kefir_ir_type *array1_type = kefir_ir_module_new_type(mem, &module, 6, NULL);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, array1_type, KEFIR_IR_TYPE_ARRAY, 0, 3));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, array1_type, KEFIR_IR_TYPE_STRUCT, 0, 2));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, array1_type, KEFIR_IR_TYPE_ARRAY, 0, 2));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, array1_type, KEFIR_IR_TYPE_CHAR, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, array1_type, KEFIR_IR_TYPE_ARRAY, 0, 2));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, array1_type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    struct kefir_ir_data *array1_data = kefir_ir_module_new_named_data(mem, &module, "array1_1", array1_type);
    REQUIRE_OK(kefir_ir_data_set_i64(array1_data, 3, 10));
    REQUIRE_OK(kefir_ir_data_set_i64(array1_data, 4, 20));
    REQUIRE_OK(kefir_ir_data_set_f32(array1_data, 6, 3.4));
    REQUIRE_OK(kefir_ir_data_set_f32(array1_data, 7, 4.5));
    REQUIRE_OK(kefir_ir_data_set_i64(array1_data, 17, 110));
    REQUIRE_OK(kefir_ir_data_set_i64(array1_data, 18, 120));
    REQUIRE_OK(kefir_ir_data_set_f32(array1_data, 20, 40.56));
    REQUIRE_OK(kefir_ir_data_set_f32(array1_data, 21, 56.74));
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, "array1_1"));

    struct kefir_ir_type *union1_type = kefir_ir_module_new_type(mem, &module, 6, NULL);
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, union1_type, KEFIR_IR_TYPE_UNION, 0, 2));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, union1_type, KEFIR_IR_TYPE_STRUCT, 0, 2));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, union1_type, KEFIR_IR_TYPE_FLOAT32, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, union1_type, KEFIR_IR_TYPE_INT32, 0, 0));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, union1_type, KEFIR_IR_TYPE_ARRAY, 0, 8));
    REQUIRE_OK(kefir_irbuilder_type_append_v(mem, union1_type, KEFIR_IR_TYPE_CHAR, 0, 0));
    struct kefir_ir_data *union1_data = kefir_ir_module_new_named_data(mem, &module, "union1_1", union1_type);
    REQUIRE_OK(kefir_ir_data_set_f32(union1_data, 2, 3.14));
    REQUIRE_OK(kefir_ir_data_set_i64(union1_data, 3, 100500));
    REQUIRE_OK(kefir_ir_module_declare_global(mem, &module, "union1_1"));

    REQUIRE_OK(KEFIR_CODEGEN_TRANSLATE(mem, &codegen.iface, &module));
    REQUIRE_OK(KEFIR_CODEGEN_CLOSE(&codegen.iface));
    REQUIRE_OK(kefir_ir_module_free(mem, &module));
    return KEFIR_OK;
}