#pragma once

#include <string>
#include <cstdint>
#include <vector>

#include <mir/mir_fwd.h>
#include <mir/mir_type.h>

// mir_operand
// 
// operands used in MIR instructions
// they can represent many different kinds of values such
// as variables, constants, function references, etc.

struct mir_const_int {
	mir_type type;
	int64_t value;
};

struct mir_const_float {
	mir_type type;
	double value;
};

struct mir_string_imm {
	mir_type type;
	std::string value;
};

struct mir_nullptr {
};

struct mir_func_ref {
	const mir_function* function;
};

struct mir_field_ref {
	uint32_t index;
};

struct mir_block_ref {
	const mir_block* block;
};

mir_type operand_type(const mir_operand& op);