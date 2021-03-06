/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2022  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kefir/ir/format_impl.h"
#include "kefir/core/util.h"
#include "kefir/core/error.h"

// Do not edit this file. It is autogenerated. Edit resources/opcodes.xml instead.
kefir_result_t kefir_ir_format_instr(struct kefir_json_output *json, const struct kefir_ir_module *module,
                                     const struct kefir_irinstr *instr) {
    REQUIRE(json != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid json output"));
    REQUIRE(module != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR module"));
    REQUIRE(instr != NULL, KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Expected valid IR instruction"));
    switch (instr->opcode) {
        case KEFIR_IROPCODE_RESERVED:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_NOP:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_JMP:
            return kefir_ir_format_instr_coderef(json, module, instr);
        case KEFIR_IROPCODE_IJMP:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_BRANCH:
            return kefir_ir_format_instr_coderef(json, module, instr);
        case KEFIR_IROPCODE_RET:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_INVOKE:
            return kefir_ir_format_instr_funcref(json, module, instr);
        case KEFIR_IROPCODE_INVOKEV:
            return kefir_ir_format_instr_funcref(json, module, instr);
        case KEFIR_IROPCODE_PUSHI64:
            return kefir_ir_format_instr_i64(json, module, instr);
        case KEFIR_IROPCODE_PUSHU64:
            return kefir_ir_format_instr_u64(json, module, instr);
        case KEFIR_IROPCODE_PUSHSTRING:
            return kefir_ir_format_instr_string(json, module, instr);
        case KEFIR_IROPCODE_PUSHLABEL:
            return kefir_ir_format_instr_u64(json, module, instr);
        case KEFIR_IROPCODE_POP:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_PICK:
            return kefir_ir_format_instr_u64(json, module, instr);
        case KEFIR_IROPCODE_PUT:
            return kefir_ir_format_instr_u64(json, module, instr);
        case KEFIR_IROPCODE_INSERT:
            return kefir_ir_format_instr_u64(json, module, instr);
        case KEFIR_IROPCODE_XCHG:
            return kefir_ir_format_instr_u64(json, module, instr);
        case KEFIR_IROPCODE_DROP:
            return kefir_ir_format_instr_u64(json, module, instr);
        case KEFIR_IROPCODE_IADD:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_IADD1:
            return kefir_ir_format_instr_i64(json, module, instr);
        case KEFIR_IROPCODE_ISUB:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_IMUL:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_IDIV:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_IMOD:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_UDIV:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_UMOD:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_INEG:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_INOT:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_IAND:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_IOR:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_IXOR:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_ILSHIFT:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_IRSHIFT:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_IARSHIFT:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_IEQUALS:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_IGREATER:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_ILESSER:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_IABOVE:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_IBELOW:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_BAND:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_BOR:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_BNOT:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_TRUNCATE1:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_EXTEND8:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_EXTEND16:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_EXTEND32:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_OFFSETPTR:
            return kefir_ir_format_instr_typeref(json, module, instr);
        case KEFIR_IROPCODE_ELEMENTPTR:
            return kefir_ir_format_instr_typeref(json, module, instr);
        case KEFIR_IROPCODE_GETGLOBAL:
            return kefir_ir_format_instr_identifier(json, module, instr);
        case KEFIR_IROPCODE_GETLOCALS:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LOAD8U:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LOAD8I:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LOAD16U:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LOAD16I:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LOAD24U:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LOAD32U:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LOAD32I:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LOAD40U:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LOAD48U:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LOAD56U:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LOAD64:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LOADLD:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_STORE8:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_STORE16:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_STORE24:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_STORE32:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_STORE40:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_STORE48:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_STORE56:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_STORE64:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_STORELD:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_BZERO:
            return kefir_ir_format_instr_typeref(json, module, instr);
        case KEFIR_IROPCODE_BCOPY:
            return kefir_ir_format_instr_typeref(json, module, instr);
        case KEFIR_IROPCODE_EXTUBITS:
            return kefir_ir_format_instr_u32(json, module, instr);
        case KEFIR_IROPCODE_EXTSBITS:
            return kefir_ir_format_instr_u32(json, module, instr);
        case KEFIR_IROPCODE_INSERTBITS:
            return kefir_ir_format_instr_u32(json, module, instr);
        case KEFIR_IROPCODE_GETTHRLOCAL:
            return kefir_ir_format_instr_identifier(json, module, instr);
        case KEFIR_IROPCODE_VARARG_START:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_VARARG_COPY:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_VARARG_GET:
            return kefir_ir_format_instr_typeref(json, module, instr);
        case KEFIR_IROPCODE_VARARG_END:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_ALLOCA:
            return kefir_ir_format_instr_bool(json, module, instr);
        case KEFIR_IROPCODE_PUSHSCOPE:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_POPSCOPE:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_PUSHF32:
            return kefir_ir_format_instr_f32(json, module, instr);
        case KEFIR_IROPCODE_PUSHF64:
            return kefir_ir_format_instr_f64(json, module, instr);
        case KEFIR_IROPCODE_PUSHLD:
            return kefir_ir_format_instr_u64(json, module, instr);
        case KEFIR_IROPCODE_F32ADD:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F32SUB:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F32MUL:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F32DIV:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F32NEG:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F64ADD:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F64SUB:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F64MUL:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F64DIV:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F64NEG:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LDADD:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LDSUB:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LDMUL:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LDDIV:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LDNEG:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F32EQUALS:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F32GREATER:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F32LESSER:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F64EQUALS:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F64GREATER:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F64LESSER:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LDEQUALS:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LDGREATER:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LDLESSER:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LDTRUNC1:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F32CINT:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F64CINT:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LDCINT:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_INTCF32:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_INTCF64:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_INTCLD:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_UINTCF32:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_UINTCF64:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_UINTCLD:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F32CF64:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F32CLD:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F64CF32:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_F64CLD:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LDCF32:
            return kefir_ir_format_instr_none(json, module, instr);
        case KEFIR_IROPCODE_LDCF64:
            return kefir_ir_format_instr_none(json, module, instr);
    }
    return KEFIR_SET_ERROR(KEFIR_INTERNAL_ERROR, "Unknown opcode");
}
