#include "mir_builder.h"

#include <representations/types/data_type.h>
#include <representations/types/function_type.h>
#include <mir/mir_block.h>
#include <mir/mir_function.h>
#include <mir/mir_value.h>
#include <mir/mir_instr.h>
#include <mir/mir_operand.h>
#include <utils/assert.h>

void mir_builder::build_ret_void() {
	insert_instr({
		.kind = mir_instr_opcode::RET,
		.type = {data_type::get(DT_VOID)}
	});
}
void mir_builder::build_ret(mir_operand value) {
	insert_instr({
		.kind = mir_instr_opcode::RET,
		.type = operand_type(value),
		.operands = {
			mir_operand{value}
		}
	});
}

mir_value mir_builder::build_add(mir_operand lhs, mir_operand rhs, const std::string& result_name) {
	return build_binary_op(mir_instr_opcode::ADD, lhs, rhs, result_name);
}
mir_value mir_builder::build_sub(mir_operand lhs, mir_operand rhs, const std::string& result_name) {
	return build_binary_op(mir_instr_opcode::SUB, lhs, rhs, result_name);
}
mir_value mir_builder::build_mul(mir_operand lhs, mir_operand rhs, const std::string& result_name) {
	return build_binary_op(mir_instr_opcode::MUL, lhs, rhs, result_name);
}
mir_value mir_builder::build_div(mir_operand lhs, mir_operand rhs, const std::string& result_name) {
	return build_binary_op(mir_instr_opcode::DIV, lhs, rhs, result_name);
}
mir_value mir_builder::build_mod(mir_operand lhs, mir_operand rhs, const std::string& result_name) {
	return build_binary_op(mir_instr_opcode::MOD, lhs, rhs, result_name);
}
mir_value mir_builder::build_binary_op(mir_instr_opcode opcode, mir_operand lhs, mir_operand rhs, const std::string& result_name) {
	s_assert(check_type_match(lhs, rhs),
		"Types don't match in binary operation");

	mir_type result_type = mir_type{nullptr};
	switch (opcode) {
	// arithmetic - use lhs
	case mir_instr_opcode::ADD:
	case mir_instr_opcode::SUB:
	case mir_instr_opcode::MUL:
	case mir_instr_opcode::DIV:
	case mir_instr_opcode::MOD:
		result_type = operand_type(lhs);
		break;
	// comparison - always bool
	case mir_instr_opcode::CMP_EQ:
	case mir_instr_opcode::CMP_NEQ:
	case mir_instr_opcode::CMP_LT:
	case mir_instr_opcode::CMP_LTE:
	case mir_instr_opcode::CMP_GT:
	case mir_instr_opcode::CMP_GTE:
		result_type = mir_type{ data_type::get(DT_BOOL) };
		break;
	// logical - always bool
	case mir_instr_opcode::AND:
	case mir_instr_opcode::OR:
	case mir_instr_opcode::NOT:
		result_type = mir_type{ data_type::get(DT_BOOL) };
		break;
	}

	s_assert(result_type.ty != nullptr,
		"Failed to identify binary operation result type");

	// both same - just use lhs
	mir_value result = mir_value(result_type, result_name);
	insert_instr({
		.kind = opcode,
		.type = result_type,
		.result = result,
		.operands = {
			mir_operand{lhs},
			mir_operand{rhs}
		}
	});
	return result;
}

mir_operand mir_builder::build_const_int(int64_t value, mir_type type) {
	return mir_const_int {
		.type = type,
		.value = value
	};
}
mir_operand mir_builder::build_const_float(double value, mir_type type) {
	return mir_const_float {
		.type = type,
		.value = value
	};
}
mir_operand mir_builder::build_const_string(const std::string& value, mir_type type) {
	return mir_string_imm {
		.type = type,
		.value = value
	};
}
mir_operand mir_builder::build_const_nullptr() {
	return mir_nullptr{};
}

void mir_builder::build_branch(mir_block* target) {
	insert_instr({
		.kind = mir_instr_opcode::BRA,
		.operands = {
			mir_block_ref{target}
		}
	});
}
void mir_builder::build_cond_branch(mir_operand condition, mir_block* true_block, mir_block* false_block) {
	s_assert(operand_type(condition).ty->is_bool(),
		"Condition operand for conditional branch must be of boolean type");

	s_assert(true_block != nullptr && false_block != nullptr,
		"Both destination blocks cannot be null in conditional branch");

	insert_instr({
		.kind = mir_instr_opcode::BRA_CND,
		.operands = {
			condition,
			mir_block_ref{true_block},
			mir_block_ref{false_block}
		}
	});
}

mir_operand mir_builder::build_call(const mir_function* func, std::vector<mir_operand> args, const std::string& result_name) {
	s_assert(func != nullptr, "Function reference cannot be null in call instruction");

	// insert function as first operand
	args.insert(args.begin(), mir_func_ref{ func });

	// create call instruction
	mir_value result = mir_value(func->return_type, result_name);
	insert_instr({
		.kind = mir_instr_opcode::CALL,
		.type = func->return_type,
		.result = result,
		.operands = args
	});
	return result;
}

mir_value mir_builder::build_cast(mir_operand value, mir_type target_type, const std::string& result_name) {
	// shouldnt need to verify casting validity here - that should be done earlier
	mir_value result = mir_value(target_type, result_name);
	insert_instr({
		.kind = mir_instr_opcode::CAST,
		.type = target_type,
		.result = result,
		.operands = {
			value
		}
	});
	return result;
}

void mir_builder::insert_instr(const mir_instr&& instr) {
	s_assert(ins_block != nullptr,
		"Cannot insert an instruction when insert block is null");

	ins_block->push_instr(instr);
}

bool mir_builder::check_type_match(mir_operand lhs, mir_operand rhs) {
	auto lty = operand_type(lhs);
	auto rty = operand_type(rhs);
	if (!(lty == rty)) {
		return false;
	}
	return true;
}
