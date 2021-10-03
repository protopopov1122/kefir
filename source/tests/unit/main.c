/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

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

#include <stdio.h>
#include "kefir/core/util.h"
#include "kefir/test/unit_test.h"
#include <assert.h>

DECLARE_CASE(amd64_sysv_abi_data_test1);
DECLARE_CASE(amd64_sysv_abi_data_test2);
DECLARE_CASE(amd64_sysv_abi_data_test3);
DECLARE_CASE(amd64_sysv_abi_data_test4);
DECLARE_CASE(amd64_sysv_abi_data_test5);
DECLARE_CASE(amd64_sysv_abi_data_test6);
DECLARE_CASE(amd64_sysv_abi_data_test7);
DECLARE_CASE(amd64_sysv_abi_data_test8);
DECLARE_CASE(amd64_sysv_abi_data_test9);
DECLARE_CASE(amd64_sysv_abi_classification_test1);
DECLARE_CASE(amd64_sysv_abi_classification_test2);
DECLARE_CASE(amd64_sysv_abi_classification_test3);
DECLARE_CASE(amd64_sysv_abi_classification_test4);
DECLARE_CASE(amd64_sysv_abi_classification_test5);
DECLARE_CASE(amd64_sysv_abi_classification_test6);
DECLARE_CASE(amd64_sysv_abi_classification_test7);
DECLARE_CASE(amd64_sysv_abi_classification_test8);
DECLARE_CASE(amd64_sysv_abi_classification_test9);
DECLARE_CASE(amd64_sysv_abi_classification_test10);
DECLARE_CASE(amd64_sysv_abi_classification_test11);
DECLARE_CASE(amd64_sysv_abi_classification_test12);
DECLARE_CASE(amd64_sysv_abi_classification_test13);
DECLARE_CASE(amd64_sysv_abi_classification_test14);
DECLARE_CASE(amd64_sysv_abi_classification_test15);
DECLARE_CASE(amd64_sysv_abi_allocation_test1);
DECLARE_CASE(amd64_sysv_abi_allocation_test2);
DECLARE_CASE(amd64_sysv_abi_allocation_test3);
DECLARE_CASE(amd64_sysv_abi_allocation_test4);
DECLARE_CASE(amd64_sysv_abi_allocation_test5);
DECLARE_CASE(amd64_sysv_abi_allocation_test6);

DECLARE_CASE(ast_type_analysis_integer_promotion1);
DECLARE_CASE(ast_type_analysis_arithmetic_conversion1);
DECLARE_CASE(ast_type_analysis_arithmetic_conversion2);
DECLARE_CASE(ast_type_analysis_arithmetic_conversion3);
DECLARE_CASE(ast_ordinary_scope_objects1);
DECLARE_CASE(ast_ordinary_scope_objects2);
DECLARE_CASE(ast_ordinary_scope_objects3);
DECLARE_CASE(ast_ordinary_scope_objects4);
DECLARE_CASE(ast_ordinary_scope_objects5);
DECLARE_CASE(ast_ordinary_scope_objects6);
DECLARE_CASE(ast_ordinary_scope_objects7);
DECLARE_CASE(ast_ordinary_scope_objects8);
DECLARE_CASE(ast_ordinary_scope_objects9);
DECLARE_CASE(ast_ordinary_scope_objects10);
DECLARE_CASE(ast_ordinary_scope_objects11);
DECLARE_CASE(ast_ordinary_scope_objects12);
DECLARE_CASE(ast_ordinary_scope_objects13);
DECLARE_CASE(ast_ordinary_scope_objects14);
DECLARE_CASE(ast_ordinary_scope_objects15);
DECLARE_CASE(ast_type_construction1);
DECLARE_CASE(ast_type_construction2);
DECLARE_CASE(ast_type_construction3);
DECLARE_CASE(ast_type_construction4);
DECLARE_CASE(ast_type_construction5);
DECLARE_CASE(ast_type_construction6);
DECLARE_CASE(ast_type_construction7);
DECLARE_CASE(ast_type_construction8);
DECLARE_CASE(ast_nodes_constants);
DECLARE_CASE(ast_nodes_identifiers);
DECLARE_CASE(ast_nodes_string_literals_multibyte);
DECLARE_CASE(ast_nodes_string_literals_unicode8);
DECLARE_CASE(ast_nodes_string_literals_unicode16);
DECLARE_CASE(ast_nodes_string_literals_unicode32);
DECLARE_CASE(ast_nodes_string_literals_wide);
DECLARE_CASE(ast_nodes_type_name);
DECLARE_CASE(ast_nodes_array_subscripts);
DECLARE_CASE(ast_nodes_function_calls);
DECLARE_CASE(ast_nodes_builtins);
DECLARE_CASE(ast_nodes_struct_members);
DECLARE_CASE(ast_nodes_unary_operations);
DECLARE_CASE(ast_nodes_binary_operations);
DECLARE_CASE(ast_nodes_generic_selections);
DECLARE_CASE(ast_nodes_cast_operators);
DECLARE_CASE(ast_nodes_conditional_operators);
DECLARE_CASE(ast_nodes_assignment_operators);
DECLARE_CASE(ast_nodes_comma_operators);
DECLARE_CASE(ast_nodes_compound_literals);
DECLARE_CASE(ast_nodes_init_declarators1);
DECLARE_CASE(ast_nodes_static_assertions1);
DECLARE_CASE(ast_nodes_labeled_statements1);
DECLARE_CASE(ast_nodes_case_statements1);
DECLARE_CASE(ast_nodes_expression_statements1);
DECLARE_CASE(ast_type_compatibility1);
DECLARE_CASE(ast_type_basic_compatibility);
DECLARE_CASE(ast_type_enum_compatibility);
DECLARE_CASE(ast_type_pointer_compatibility);
DECLARE_CASE(ast_type_qualified_compatibility);
DECLARE_CASE(ast_type_struct_compatibility);
DECLARE_CASE(ast_type_union_compatibility);
DECLARE_CASE(ast_type_array_compatibility);
DECLARE_CASE(ast_type_function_compatibility);
DECLARE_CASE(ast_type_basic_composite);
DECLARE_CASE(ast_type_pointer_composite);
DECLARE_CASE(ast_type_enum_composite);
DECLARE_CASE(ast_type_qualified_composite);
DECLARE_CASE(ast_type_struct_composite);
DECLARE_CASE(ast_type_union_composite);
DECLARE_CASE(ast_type_array_composite);
DECLARE_CASE(ast_type_function_composite);
DECLARE_CASE(ast_type_scope1);
DECLARE_CASE(ast_type_scope2);
DECLARE_CASE(ast_type_scope3);
DECLARE_CASE(ast_ordinary_constant_scope1);
DECLARE_CASE(ast_ordinary_typedef_scope1);
DECLARE_CASE(ast_ordinary_scope_composite_objects_external);
DECLARE_CASE(ast_ordinary_scope_composite_objects_external_declaration);
DECLARE_CASE(ast_ordinary_scope_composite_objects_thread_local_external);
DECLARE_CASE(ast_ordinary_scope_composite_objects_thread_local_external_declaration);
DECLARE_CASE(ast_ordinary_scope_composite_objects_static);
DECLARE_CASE(ast_ordinary_scope_composite_objects_static_thread_local);
DECLARE_CASE(ast_ordinary_scope_composite_objects_local_external);
DECLARE_CASE(ast_ordinary_scope_composite_objects_local_thread_local_external);
DECLARE_CASE(ast_ordinary_function_scope1);
DECLARE_CASE(ast_ordinary_function_scope2);
DECLARE_CASE(ast_ordinary_function_scope3);
DECLARE_CASE(ast_ordinary_function_scope4);
DECLARE_CASE(ast_ordinary_function_scope5);
DECLARE_CASE(ast_ordinary_scope_objects_init1);
DECLARE_CASE(ast_ordinary_scope_objects_init2);
DECLARE_CASE(ast_ordinary_scope_objects_init3);
DECLARE_CASE(ast_ordinary_scope_objects_init4);
DECLARE_CASE(ast_ordinary_scope_objects_init5);
DECLARE_CASE(ast_ordinary_scope_objects_init6);
DECLARE_CASE(ast_ordinary_scope_objects_init7);
DECLARE_CASE(ast_ordinary_scope_objects_init8);
DECLARE_CASE(ast_ordinary_scope_objects_local_init1);
DECLARE_CASE(ast_ordinary_scope_objects_local_init2);
DECLARE_CASE(ast_ordinary_scope_objects_local_init3);
DECLARE_CASE(ast_ordinary_scope_objects_local_init4);
DECLARE_CASE(ast_node_analysis_constants);
DECLARE_CASE(ast_node_analysis_string_literals_multibyte);
DECLARE_CASE(ast_node_analysis_string_literals_unicode8);
DECLARE_CASE(ast_node_analysis_string_literals_unicode16);
DECLARE_CASE(ast_node_analysis_string_literals_unicode32);
DECLARE_CASE(ast_node_analysis_string_literals_wide);
DECLARE_CASE(ast_node_analysis_identifiers);
DECLARE_CASE(ast_node_analysis_array_subscripts);
DECLARE_CASE(ast_node_analysis_struct_members1);
DECLARE_CASE(ast_node_analysis_struct_members2);
DECLARE_CASE(ast_node_analysis_indirect_struct_members1);
DECLARE_CASE(ast_node_analysis_indirect_struct_members2);
DECLARE_CASE(ast_node_analysis_function_calls);
DECLARE_CASE(ast_node_analysis_function_calls1);
DECLARE_CASE(ast_node_analysis_function_calls2);
DECLARE_CASE(ast_node_analysis_function_calls3);
DECLARE_CASE(ast_node_analysis_function_calls4);
DECLARE_CASE(ast_node_analysis_builtins1);
DECLARE_CASE(ast_node_analysis_builtins2);
DECLARE_CASE(ast_node_analysis_builtins3);
DECLARE_CASE(ast_node_analysis_builtins4);
DECLARE_CASE(ast_node_analysis_unary_operation_arithmetic);
DECLARE_CASE(ast_node_analysis_unary_operation_address);
DECLARE_CASE(ast_node_analysis_unary_operation_indirect);
DECLARE_CASE(ast_node_analysis_unary_operation_incdec);
DECLARE_CASE(ast_node_analysis_unary_operation_sizeof);
DECLARE_CASE(ast_node_analysis_unary_operation_alignof);
DECLARE_CASE(ast_node_analysis_type_name);
DECLARE_CASE(ast_node_analysis_generic_selections);
DECLARE_CASE(ast_node_analysis_cast_operators);
DECLARE_CASE(ast_node_analysis_multiplicative_operators);
DECLARE_CASE(ast_node_analysis_add_operator);
DECLARE_CASE(ast_node_analysis_subtract_operator);
DECLARE_CASE(ast_node_analysis_shift_operator);
DECLARE_CASE(ast_node_analysis_relational_operators);
DECLARE_CASE(ast_node_analysis_equality_operators);
DECLARE_CASE(ast_node_analysis_bitwise_operators);
DECLARE_CASE(ast_node_analysis_logical_operators);
DECLARE_CASE(ast_node_analysis_conditional_operator1);
DECLARE_CASE(ast_node_analysis_conditional_operator2);
DECLARE_CASE(ast_node_analysis_conditional_operator3);
DECLARE_CASE(ast_node_analysis_conditional_operator4);
DECLARE_CASE(ast_node_analysis_conditional_operator5);
DECLARE_CASE(ast_node_analysis_simple_assignment_operator1);
DECLARE_CASE(ast_node_analysis_simple_assignment_operator2);
DECLARE_CASE(ast_node_analysis_simple_assignment_operator3);
DECLARE_CASE(ast_node_analysis_simple_assignment_operator4);
DECLARE_CASE(ast_node_analysis_simple_assignment_operator5);
DECLARE_CASE(ast_node_analysis_simple_assignment_operator6);
DECLARE_CASE(ast_node_analysis_simple_assignment_operator7);
DECLARE_CASE(ast_node_analysis_compound_assignment_operator1);
DECLARE_CASE(ast_node_analysis_compound_assignment_operator2);
DECLARE_CASE(ast_node_analysis_compound_assignment_operator3);
DECLARE_CASE(ast_nodes_comma_operators);
DECLARE_CASE(ast_node_analysis_compound_literal1);
DECLARE_CASE(ast_node_analysis_compound_literal2);
DECLARE_CASE(ast_node_analysis_compound_literal3);
DECLARE_CASE(ast_node_analysis_compound_literal4);
DECLARE_CASE(ast_type_traversal1);
DECLARE_CASE(ast_type_traversal2);
DECLARE_CASE(ast_type_traversal3);
DECLARE_CASE(ast_type_traversal4);
DECLARE_CASE(ast_type_traversal5);
DECLARE_CASE(ast_initializer_construction1);
DECLARE_CASE(ast_initializer_construction2);
DECLARE_CASE(ast_initializer_analysis1);
DECLARE_CASE(ast_initializer_analysis2);
DECLARE_CASE(ast_initializer_analysis3);
DECLARE_CASE(ast_initializer_analysis4);
DECLARE_CASE(ast_initializer_analysis5);
DECLARE_CASE(ast_initializer_analysis_unicode8);
DECLARE_CASE(ast_initializer_analysis_unicode16);
DECLARE_CASE(ast_initializer_analysis_unicode32);
DECLARE_CASE(ast_initializer_analysis_wide);
DECLARE_CASE(ast_initializer_analysis_mixed_strings);
DECLARE_CASE(ast_constant_expression_constant1);
DECLARE_CASE(ast_constant_expression_identifier1);
DECLARE_CASE(ast_constant_expression_string_literal1);
DECLARE_CASE(ast_constant_expression_generic_selection1);
DECLARE_CASE(ast_constant_expression_unary_operations1);
DECLARE_CASE(ast_constant_expression_unary_operations2);
DECLARE_CASE(ast_constant_expression_unary_operations3);
DECLARE_CASE(ast_constant_expression_unary_operations4);
DECLARE_CASE(ast_constant_expression_unary_operations5);
DECLARE_CASE(ast_constant_expression_unary_operations6);
DECLARE_CASE(ast_constant_expression_unary_operations7);
DECLARE_CASE(ast_constant_expression_binary_operations1);
DECLARE_CASE(ast_constant_expression_binary_operations2);
DECLARE_CASE(ast_constant_expression_binary_operations3);
DECLARE_CASE(ast_constant_expression_binary_operations4);
DECLARE_CASE(ast_constant_expression_conditional_operator1);
DECLARE_CASE(ast_constant_expression_conditional_operator2);
DECLARE_CASE(ast_constant_expression_cast_operator1);
DECLARE_CASE(ast_constant_expression_cast_operator2);
DECLARE_CASE(ast_translator_environment1);
DECLARE_CASE(ast_translator_environment2);
DECLARE_CASE(ast_alignment_default);
DECLARE_CASE(ast_alignment_as_type);
DECLARE_CASE(ast_alignment_as_const_expr);
DECLARE_CASE(ast_type_analysis_scalars);
DECLARE_CASE(ast_type_analysis_pointers_qualifications);
DECLARE_CASE(ast_type_analysis_enumerations);
DECLARE_CASE(ast_type_analysis_arrays);
DECLARE_CASE(ast_type_analysis_structs);
DECLARE_CASE(ast_type_analysis_functions);
DECLARE_CASE(ast_type_analysis_flexible_arrays);
DECLARE_CASE(ast_bitfield_translator1);
DECLARE_CASE(ast_bitfield_translator2);
DECLARE_CASE(ast_bitfield_translator3);
DECLARE_CASE(ast_bitfield_translator4);
DECLARE_CASE(ast_bitfield_translator5);
DECLARE_CASE(ast_declarator_specifier_construction1);
DECLARE_CASE(ast_declarator_specifier_construction2);
DECLARE_CASE(ast_declarator_specifier_construction3);
DECLARE_CASE(ast_declarator_specifier_construction4);
DECLARE_CASE(ast_declarator_specifier_construction5);
DECLARE_CASE(ast_declarator_specifier_construction6);
DECLARE_CASE(ast_declarator_specifier_construction7);
DECLARE_CASE(ast_declarator_specifier_construction8);
DECLARE_CASE(ast_declarator_specifier_construction9);
DECLARE_CASE(ast_declarator_specifier_construction10);
DECLARE_CASE(ast_declarator_specifier_construction11);
DECLARE_CASE(ast_declarator_construction1);
DECLARE_CASE(ast_declarator_construction2);
DECLARE_CASE(ast_declarator_construction3);
DECLARE_CASE(ast_declarator_construction4);
DECLARE_CASE(ast_declarator_construction5);
DECLARE_CASE(ast_declarator_construction6);
DECLARE_CASE(ast_declarator_construction7);
DECLARE_CASE(ast_declarator_construction8);
DECLARE_CASE(ast_declarator_construction9);
DECLARE_CASE(ast_declarator_construction10);
DECLARE_CASE(ast_declarator_construction11);
DECLARE_CASE(ast_structure_declaration1);
DECLARE_CASE(ast_structure_declaration2);
DECLARE_CASE(ast_enum_declaration1);
DECLARE_CASE(ast_enum_declaration2);
DECLARE_CASE(ast_declarator_analysis1);
DECLARE_CASE(ast_declarator_analysis2);
DECLARE_CASE(ast_declarator_analysis3);
DECLARE_CASE(ast_declarator_analysis4);
DECLARE_CASE(ast_declarator_analysis5);
DECLARE_CASE(ast_declarator_analysis6);
DECLARE_CASE(ast_declarator_analysis7);
DECLARE_CASE(ast_declarator_analysis8);
DECLARE_CASE(ast_declarator_analysis9);
DECLARE_CASE(ast_declarator_analysis10);
DECLARE_CASE(ast_declarator_analysis11);
DECLARE_CASE(ast_declarator_analysis12);
DECLARE_CASE(ast_declarator_analysis13);
DECLARE_CASE(ast_declarator_analysis14);
DECLARE_CASE(ast_declarator_analysis15);
DECLARE_CASE(ast_declarator_analysis16);
DECLARE_CASE(ast_declarator_analysis17);
DECLARE_CASE(ast_declarator_analysis18);
DECLARE_CASE(ast_declarator_analysis19);
DECLARE_CASE(ast_declarator_analysis20);
DECLARE_CASE(ast_declarator_analysis21);
DECLARE_CASE(ast_declarator_analysis22);
DECLARE_CASE(ast_declarator_analysis23);
DECLARE_CASE(ast_declarator_analysis24);
DECLARE_CASE(ast_declarator_analysis25);
DECLARE_CASE(ast_node_analysis_init_declarators1);
DECLARE_CASE(ast_node_analysis_init_declarators2);
DECLARE_CASE(ast_node_analysis_init_declarators3);
DECLARE_CASE(ast_node_analysis_init_declarators4);
DECLARE_CASE(ast_node_analysis_init_declarators5);
DECLARE_CASE(ast_node_analysis_init_declarators6);
DECLARE_CASE(ast_node_analysis_init_declarators7);
DECLARE_CASE(ast_node_analysis_init_declarators8);
DECLARE_CASE(ast_node_analysis_init_declarators9);
DECLARE_CASE(ast_node_analysis_init_declarators10);
DECLARE_CASE(ast_node_analysis_init_declarators11);
DECLARE_CASE(ast_node_analysis_init_declarators12);
DECLARE_CASE(ast_node_analysis_init_declarators13);
DECLARE_CASE(ast_node_analysis_static_assertions1);
DECLARE_CASE(ast_node_analysis_expression_statements1);
DECLARE_CASE(ast_node_analysis_labeled_statements1);
DECLARE_CASE(ast_node_analysis_case_statements1);
DECLARE_CASE(ast_nodes_compound_statements1);
DECLARE_CASE(ast_nodes_conditional_statements1);
DECLARE_CASE(ast_label_scope1);
DECLARE_CASE(ast_label_scope2);
DECLARE_CASE(ast_label_scope3);
DECLARE_CASE(ast_label_scope4);
DECLARE_CASE(ast_node_analysis_labeled_statements2);
DECLARE_CASE(ast_node_analysis_compound_statements1);
DECLARE_CASE(ast_node_analysis_compound_statements2);
DECLARE_CASE(ast_node_analysis_compound_statements3);
DECLARE_CASE(ast_node_analysis_compound_statements4);
DECLARE_CASE(valueast_node_analysis_conditional_statements1);
DECLARE_CASE(ast_node_analysis_conditinal_statements2);
DECLARE_CASE(ast_node_analysis_conditinal_statements3);
DECLARE_CASE(ast_flow_control_tree1);
DECLARE_CASE(ast_flow_control_tree2);
DECLARE_CASE(ast_flow_control_tree3);
DECLARE_CASE(ast_flow_control_tree4);
DECLARE_CASE(ast_flow_control_tree5);
DECLARE_CASE(ast_flow_control_tree6);
DECLARE_CASE(ast_nodes_switch_statements1);
DECLARE_CASE(ast_node_analysis_switch_statements1);
DECLARE_CASE(ast_node_analysis_switch_statements2);
DECLARE_CASE(ast_node_analysis_switch_statements3);
DECLARE_CASE(ast_node_analysis_switch_statements4);
DECLARE_CASE(ast_node_analysis_switch_statements5);
DECLARE_CASE(ast_nodes_while_statements1);
DECLARE_CASE(ast_nodes_while_statements2);
DECLARE_CASE(ast_nodes_do_while_statements1);
DECLARE_CASE(ast_nodes_do_while_statements2);
DECLARE_CASE(ast_nodes_for_statements1);
DECLARE_CASE(ast_nodes_for_statements2);
DECLARE_CASE(ast_nodes_for_statements3);
DECLARE_CASE(ast_nodes_for_statements4);
DECLARE_CASE(ast_nodes_for_statements5);
DECLARE_CASE(ast_node_analysis_while_statements1);
DECLARE_CASE(ast_node_analysis_while_statements2);
DECLARE_CASE(ast_node_analysis_do_while_statements1);
DECLARE_CASE(ast_node_analysis_do_while_statements2);
DECLARE_CASE(ast_node_analysis_for_statements1);
DECLARE_CASE(ast_node_analysis_for_statements2);
DECLARE_CASE(ast_node_analysis_for_statements3);
DECLARE_CASE(ast_nodes_goto_statements1);
DECLARE_CASE(ast_nodes_continue_statements);
DECLARE_CASE(ast_nodes_break_statements);
DECLARE_CASE(ast_nodes_return_statements1);
DECLARE_CASE(ast_node_analysis_goto_statements1);
DECLARE_CASE(ast_node_analysis_break_statements1);
DECLARE_CASE(ast_node_analysis_break_statements2);
DECLARE_CASE(ast_node_analysis_break_statements3);
DECLARE_CASE(ast_node_analysis_break_statements4);
DECLARE_CASE(ast_node_analysis_break_statements5);
DECLARE_CASE(ast_node_analysis_break_statements6);
DECLARE_CASE(ast_node_analysis_continue_statements1);
DECLARE_CASE(ast_node_analysis_continue_statements2);
DECLARE_CASE(ast_node_analysis_continue_statements3);
DECLARE_CASE(ast_node_analysis_continue_statements4);
DECLARE_CASE(ast_node_analysis_continue_statements5);
DECLARE_CASE(ast_node_analysis_return_statements1);
DECLARE_CASE(ast_translator_flow_control_point1);
DECLARE_CASE(ast_nodes_function_definitions1);
DECLARE_CASE(ast_nodes_translation_units1);
DECLARE_CASE(ast_node_analysis_function_definitions1);
DECLARE_CASE(ast_node_analysis_function_definitions2);
DECLARE_CASE(ast_node_analysis_function_definitions3);
DECLARE_CASE(ast_node_analysis_translation_unit1);
DECLARE_CASE(ast_node_analysis_translation_unit2);
DECLARE_CASE(ast_nodes_declaration1);
DECLARE_CASE(ast_node_analysis_declaration1);

DECLARE_CASE(parser_lexem_construction_sentinel);
DECLARE_CASE(parser_lexem_construction_keyword);
DECLARE_CASE(parser_lexem_construction_identifier);
DECLARE_CASE(parser_lexem_construction_constants);
DECLARE_CASE(parser_lexem_construction_string_literals);
DECLARE_CASE(parser_lexem_construction_unicode8_string_literals);
DECLARE_CASE(parser_lexem_construction_unicode16_string_literals);
DECLARE_CASE(parser_lexem_construction_unicode32_string_literals);
DECLARE_CASE(parser_lexem_construction_wide_string_literals);
DECLARE_CASE(parser_lexem_construction_punctuator);
DECLARE_CASE(parser_lexem_construction_pp_whitespace);
DECLARE_CASE(parser_lexem_construction_pp_numbers);
DECLARE_CASE(parser_lexem_construction_pp_header_name);
DECLARE_CASE(parser_lexem_move);
DECLARE_CASE(parser_lexem_copy);
DECLARE_CASE(parser_token_cursor1);
DECLARE_CASE(parser_block_scope1);
DECLARE_CASE(parser_scope1);

DECLARE_CASE(lexer_source_cursor1);
DECLARE_CASE(lexer_source_location1);
DECLARE_CASE(lexer_source_cursor_newlines);

DECLARE_CASE(preprocessor_macro_construction1);
DECLARE_CASE(preprocessor_macro_user_scope1);

DECLARE_CASE(cli_options1);
DECLARE_CASE(cli_options2);
DECLARE_CASE(cli_options3);
DECLARE_CASE(cli_options4);

TEST_SUITE(
    mainSuite, &amd64_sysv_abi_data_test1, &amd64_sysv_abi_data_test2, &amd64_sysv_abi_data_test3,
    &amd64_sysv_abi_data_test4, &amd64_sysv_abi_data_test5, &amd64_sysv_abi_data_test6, &amd64_sysv_abi_data_test7,
    &amd64_sysv_abi_data_test8, &amd64_sysv_abi_data_test9, &amd64_sysv_abi_classification_test1,
    &amd64_sysv_abi_classification_test2, &amd64_sysv_abi_classification_test3, &amd64_sysv_abi_classification_test4,
    &amd64_sysv_abi_classification_test5, &amd64_sysv_abi_classification_test6, &amd64_sysv_abi_classification_test7,
    &amd64_sysv_abi_classification_test8, &amd64_sysv_abi_classification_test9, &amd64_sysv_abi_classification_test10,
    &amd64_sysv_abi_classification_test11, &amd64_sysv_abi_classification_test12, &amd64_sysv_abi_classification_test13,
    &amd64_sysv_abi_classification_test14, &amd64_sysv_abi_classification_test15, &amd64_sysv_abi_allocation_test1,
    &amd64_sysv_abi_allocation_test2, &amd64_sysv_abi_allocation_test3, &amd64_sysv_abi_allocation_test4,
    &amd64_sysv_abi_allocation_test5, &amd64_sysv_abi_allocation_test6,

    &ast_type_analysis_integer_promotion1, &ast_type_analysis_arithmetic_conversion1,
    &ast_type_analysis_arithmetic_conversion2, &ast_type_analysis_arithmetic_conversion3, &ast_ordinary_scope_objects1,
    &ast_ordinary_scope_objects2, &ast_ordinary_scope_objects3, &ast_ordinary_scope_objects4,
    &ast_ordinary_scope_objects5, &ast_ordinary_scope_objects6, &ast_ordinary_scope_objects7,
    &ast_ordinary_scope_objects8, &ast_ordinary_scope_objects9, &ast_ordinary_scope_objects10,
    &ast_ordinary_scope_objects11, &ast_ordinary_scope_objects12, &ast_ordinary_scope_objects13,
    &ast_ordinary_scope_objects14, &ast_ordinary_scope_objects15, &ast_type_construction1, &ast_type_construction2,
    &ast_type_construction3, &ast_type_construction4, &ast_type_construction5, &ast_type_construction6,
    &ast_type_construction7, &ast_type_construction8, &ast_nodes_constants, &ast_nodes_identifiers,
    &ast_nodes_string_literals_multibyte, &ast_nodes_string_literals_unicode8, &ast_nodes_string_literals_unicode16,
    &ast_nodes_string_literals_unicode32, &ast_nodes_string_literals_wide, &ast_nodes_type_name,
    &ast_nodes_array_subscripts, &ast_nodes_function_calls, &ast_nodes_builtins, &ast_nodes_struct_members,
    &ast_nodes_unary_operations, &ast_nodes_binary_operations, &ast_nodes_generic_selections, &ast_nodes_cast_operators,
    &ast_nodes_conditional_operators, &ast_nodes_assignment_operators, &ast_nodes_comma_operators,
    &ast_nodes_compound_literals, &ast_nodes_init_declarators1, &ast_nodes_static_assertions1,
    &ast_nodes_labeled_statements1, &ast_nodes_case_statements1, &ast_nodes_expression_statements1,
    &ast_type_compatibility1, &ast_type_basic_compatibility, &ast_type_enum_compatibility,
    &ast_type_pointer_compatibility, &ast_type_qualified_compatibility, &ast_type_struct_compatibility,
    &ast_type_union_compatibility, &ast_type_array_compatibility, &ast_type_function_compatibility,
    &ast_type_basic_composite, &ast_type_pointer_composite, &ast_type_enum_composite, &ast_type_qualified_composite,
    &ast_type_struct_composite, &ast_type_union_composite, &ast_type_array_composite, &ast_type_function_composite,
    &ast_type_scope1, &ast_type_scope2, &ast_type_scope3, &ast_ordinary_constant_scope1, &ast_ordinary_typedef_scope1,
    &ast_ordinary_scope_composite_objects_external, &ast_ordinary_scope_composite_objects_external_declaration,
    &ast_ordinary_scope_composite_objects_thread_local_external,
    &ast_ordinary_scope_composite_objects_thread_local_external_declaration,
    &ast_ordinary_scope_composite_objects_static, &ast_ordinary_scope_composite_objects_static_thread_local,
    &ast_ordinary_scope_composite_objects_local_external,
    &ast_ordinary_scope_composite_objects_local_thread_local_external, &ast_ordinary_function_scope1,
    &ast_ordinary_function_scope2, &ast_ordinary_function_scope3, &ast_ordinary_function_scope4,
    &ast_ordinary_function_scope5, &ast_ordinary_scope_objects_init1, &ast_ordinary_scope_objects_init2,
    &ast_ordinary_scope_objects_init3, &ast_ordinary_scope_objects_init4, &ast_ordinary_scope_objects_init5,
    &ast_ordinary_scope_objects_init6, &ast_ordinary_scope_objects_init7, &ast_ordinary_scope_objects_init8,
    &ast_ordinary_scope_objects_local_init1, &ast_ordinary_scope_objects_local_init2,
    &ast_ordinary_scope_objects_local_init3, &ast_ordinary_scope_objects_local_init4, &ast_node_analysis_constants,
    &ast_node_analysis_string_literals_multibyte, &ast_node_analysis_string_literals_unicode8,
    &ast_node_analysis_string_literals_unicode16, &ast_node_analysis_string_literals_unicode32,
    &ast_node_analysis_string_literals_wide, &ast_node_analysis_identifiers, &ast_node_analysis_array_subscripts,
    &ast_node_analysis_struct_members1, &ast_node_analysis_struct_members2, &ast_node_analysis_indirect_struct_members1,
    &ast_node_analysis_indirect_struct_members2, &ast_node_analysis_function_calls, &ast_node_analysis_function_calls1,
    &ast_node_analysis_function_calls2, &ast_node_analysis_function_calls3, &ast_node_analysis_function_calls4,
    &ast_node_analysis_builtins1, &ast_node_analysis_builtins2, &ast_node_analysis_builtins3,
    &ast_node_analysis_builtins4, &ast_node_analysis_unary_operation_arithmetic,
    &ast_node_analysis_unary_operation_address, &ast_node_analysis_unary_operation_indirect,
    &ast_node_analysis_unary_operation_incdec, &ast_node_analysis_unary_operation_sizeof,
    &ast_node_analysis_unary_operation_alignof, &ast_node_analysis_type_name, &ast_node_analysis_generic_selections,
    &ast_node_analysis_cast_operators, &ast_node_analysis_multiplicative_operators, &ast_node_analysis_add_operator,
    &ast_node_analysis_subtract_operator, &ast_node_analysis_shift_operator, &ast_node_analysis_relational_operators,
    &ast_node_analysis_equality_operators, &ast_node_analysis_bitwise_operators, &ast_node_analysis_logical_operators,
    &ast_node_analysis_conditional_operator1, &ast_node_analysis_conditional_operator2,
    &ast_node_analysis_conditional_operator3, &ast_node_analysis_conditional_operator4,
    &ast_node_analysis_conditional_operator5, &ast_node_analysis_simple_assignment_operator1,
    &ast_node_analysis_simple_assignment_operator2, &ast_node_analysis_simple_assignment_operator3,
    &ast_node_analysis_simple_assignment_operator4, &ast_node_analysis_simple_assignment_operator5,
    &ast_node_analysis_simple_assignment_operator6, &ast_node_analysis_simple_assignment_operator7,
    &ast_node_analysis_compound_assignment_operator1, &ast_node_analysis_compound_assignment_operator2,
    &ast_node_analysis_compound_assignment_operator3, &ast_nodes_comma_operators, &ast_node_analysis_compound_literal1,
    &ast_node_analysis_compound_literal2, &ast_node_analysis_compound_literal3, &ast_node_analysis_compound_literal4,
    &ast_type_traversal1, &ast_type_traversal2, &ast_type_traversal3, &ast_type_traversal4, &ast_type_traversal5,
    &ast_initializer_construction1, &ast_initializer_construction2, &ast_initializer_analysis1,
    &ast_initializer_analysis2, &ast_initializer_analysis3, &ast_initializer_analysis4, &ast_initializer_analysis5,
    &ast_initializer_analysis_unicode8, &ast_initializer_analysis_unicode16, &ast_initializer_analysis_unicode32,
    &ast_initializer_analysis_wide, &ast_initializer_analysis_mixed_strings, &ast_constant_expression_constant1,
    &ast_constant_expression_identifier1, &ast_constant_expression_string_literal1,
    &ast_constant_expression_generic_selection1, &ast_constant_expression_unary_operations1,
    &ast_constant_expression_unary_operations2, &ast_constant_expression_unary_operations3,
    &ast_constant_expression_unary_operations4, &ast_constant_expression_unary_operations5,
    &ast_constant_expression_unary_operations6, &ast_constant_expression_unary_operations7,
    &ast_constant_expression_binary_operations1, &ast_constant_expression_binary_operations2,
    &ast_constant_expression_binary_operations3, &ast_constant_expression_binary_operations4,
    &ast_constant_expression_conditional_operator1, &ast_constant_expression_conditional_operator2,
    &ast_constant_expression_cast_operator1, &ast_constant_expression_cast_operator2, &ast_translator_environment1,
    &ast_translator_environment2, &ast_alignment_default, &ast_alignment_as_type, &ast_alignment_as_const_expr,
    &ast_type_analysis_scalars, &ast_type_analysis_pointers_qualifications, &ast_type_analysis_enumerations,
    &ast_type_analysis_arrays, &ast_type_analysis_structs, &ast_type_analysis_functions,
    &ast_type_analysis_flexible_arrays, &ast_bitfield_translator1, &ast_bitfield_translator2, &ast_bitfield_translator3,
    &ast_bitfield_translator4, &ast_bitfield_translator5, &ast_declarator_specifier_construction1,
    &ast_declarator_specifier_construction2, &ast_declarator_specifier_construction3,
    &ast_declarator_specifier_construction4, &ast_declarator_specifier_construction5,
    &ast_declarator_specifier_construction6, &ast_declarator_specifier_construction7,
    &ast_declarator_specifier_construction8, &ast_declarator_specifier_construction9,
    &ast_declarator_specifier_construction10, &ast_declarator_specifier_construction11, &ast_declarator_construction1,
    &ast_declarator_construction2, &ast_declarator_construction3, &ast_declarator_construction4,
    &ast_declarator_construction5, &ast_declarator_construction6, &ast_declarator_construction7,
    &ast_declarator_construction8, &ast_declarator_construction9, &ast_declarator_construction10,
    &ast_declarator_construction11, &ast_structure_declaration1, &ast_structure_declaration2, &ast_enum_declaration1,
    &ast_enum_declaration2, &ast_declarator_analysis1, &ast_declarator_analysis2, &ast_declarator_analysis3,
    &ast_declarator_analysis4, &ast_declarator_analysis5, &ast_declarator_analysis6, &ast_declarator_analysis7,
    &ast_declarator_analysis8, &ast_declarator_analysis9, &ast_declarator_analysis10, &ast_declarator_analysis11,
    &ast_declarator_analysis12, &ast_declarator_analysis13, &ast_declarator_analysis14, &ast_declarator_analysis15,
    &ast_declarator_analysis16, &ast_declarator_analysis17, &ast_declarator_analysis18, &ast_declarator_analysis19,
    &ast_declarator_analysis20, &ast_declarator_analysis21, &ast_declarator_analysis22, &ast_declarator_analysis23,
    &ast_declarator_analysis24, &ast_declarator_analysis25, &ast_node_analysis_init_declarators1,
    &ast_node_analysis_init_declarators2, &ast_node_analysis_init_declarators3, &ast_node_analysis_init_declarators4,
    &ast_node_analysis_init_declarators5, &ast_node_analysis_init_declarators6, &ast_node_analysis_init_declarators7,
    &ast_node_analysis_init_declarators8, &ast_node_analysis_init_declarators9, &ast_node_analysis_init_declarators10,
    &ast_node_analysis_init_declarators11, &ast_node_analysis_init_declarators12, &ast_node_analysis_init_declarators13,
    &ast_node_analysis_static_assertions1, &ast_node_analysis_expression_statements1,
    &ast_node_analysis_labeled_statements1, &ast_node_analysis_case_statements1, &ast_label_scope1, &ast_label_scope2,
    &ast_label_scope3, &ast_label_scope4, &ast_node_analysis_labeled_statements2, &ast_nodes_compound_statements1,
    &ast_node_analysis_compound_statements1, &ast_node_analysis_compound_statements2,
    &ast_node_analysis_compound_statements3, &ast_node_analysis_compound_statements4,
    &ast_nodes_conditional_statements1, &valueast_node_analysis_conditional_statements1,
    &ast_node_analysis_conditinal_statements2, &ast_node_analysis_conditinal_statements3, &ast_flow_control_tree1,
    &ast_flow_control_tree2, &ast_flow_control_tree3, &ast_flow_control_tree4, &ast_flow_control_tree5,
    &ast_flow_control_tree6, &ast_nodes_switch_statements1, &ast_node_analysis_switch_statements1,
    &ast_node_analysis_switch_statements2, &ast_node_analysis_switch_statements3, &ast_node_analysis_switch_statements4,
    &ast_node_analysis_switch_statements5, &ast_nodes_while_statements1, &ast_nodes_while_statements2,
    &ast_nodes_do_while_statements1, &ast_nodes_do_while_statements2, &ast_nodes_for_statements1,
    &ast_nodes_for_statements2, &ast_nodes_for_statements3, &ast_nodes_for_statements4, &ast_nodes_for_statements5,
    &ast_node_analysis_while_statements1, &ast_node_analysis_while_statements2, &ast_node_analysis_do_while_statements1,
    &ast_node_analysis_do_while_statements2, &ast_node_analysis_for_statements1, &ast_node_analysis_for_statements2,
    &ast_node_analysis_for_statements3, &ast_nodes_goto_statements1, &ast_nodes_continue_statements,
    &ast_nodes_break_statements, &ast_nodes_return_statements1, &ast_node_analysis_goto_statements1,
    &ast_node_analysis_break_statements1, &ast_node_analysis_break_statements2, &ast_node_analysis_break_statements3,
    &ast_node_analysis_break_statements4, &ast_node_analysis_break_statements5, &ast_node_analysis_break_statements6,
    &ast_node_analysis_continue_statements1, &ast_node_analysis_continue_statements2,
    &ast_node_analysis_continue_statements3, &ast_node_analysis_continue_statements4,
    &ast_node_analysis_continue_statements5, &ast_node_analysis_return_statements1, &ast_translator_flow_control_point1,
    &ast_nodes_function_definitions1, &ast_nodes_translation_units1, &ast_node_analysis_function_definitions1,
    &ast_node_analysis_function_definitions2, &ast_node_analysis_function_definitions3,
    &ast_node_analysis_translation_unit1, &ast_node_analysis_translation_unit2, &ast_nodes_declaration1,
    &ast_node_analysis_declaration1,

    &parser_lexem_construction_sentinel, &parser_lexem_construction_keyword, &parser_lexem_construction_identifier,
    &parser_lexem_construction_constants, &parser_lexem_construction_string_literals,
    &parser_lexem_construction_unicode8_string_literals, &parser_lexem_construction_unicode16_string_literals,
    &parser_lexem_construction_unicode32_string_literals, &parser_lexem_construction_wide_string_literals,
    &parser_lexem_construction_punctuator, &parser_lexem_construction_pp_whitespace,
    &parser_lexem_construction_pp_numbers, &parser_lexem_construction_pp_header_name, &parser_lexem_move,
    &parser_lexem_copy, &parser_token_cursor1, &parser_block_scope1, &parser_scope1,

    &lexer_source_cursor1, &lexer_source_location1, &lexer_source_cursor_newlines,

    &preprocessor_macro_construction1, &preprocessor_macro_user_scope1,

    &cli_options1, &cli_options2, &cli_options3, &cli_options4);

int main(int argc, const char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    return kft_run_test_suite(mainSuite, mainSuiteLength, NULL);
}
