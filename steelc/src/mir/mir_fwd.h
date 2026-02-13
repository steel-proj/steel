#pragma once

#include <variant>

// forward declarations for all mir types

struct mir_module;
class mir_function;
class mir_block;
struct mir_instr;
class mir_value;
struct mir_type;

enum class mir_instr_opcode;

struct mir_const_int;
struct mir_const_float;
struct mir_string_imm;
struct mir_nullptr;
struct mir_func_ref;
struct mir_field_ref;
struct mir_block_ref;

using mir_operand = std::variant<
	mir_value,
	mir_const_int,
	mir_const_float,
	mir_string_imm,
	mir_nullptr,
	mir_func_ref,
	mir_field_ref,
	mir_block_ref
>;
