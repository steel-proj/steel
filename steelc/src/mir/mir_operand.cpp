#include "mir_operand.h"

#include <variant>
#include <type_traits>

#include <mir/mir_value.h>
#include <mir/mir_function.h>
#include <mir/mir_type.h>
#include <mir/mir_block.h>
#include <mir/mir_instr.h>

mir_type operand_type(const mir_operand& op) {
	return std::visit([](auto&& arg) -> mir_type {
		using T = std::decay_t<decltype(arg)>;
		if constexpr (std::is_same_v<T, mir_value>) {
			return arg.get_type();
		}
		else if constexpr (std::is_same_v<T, mir_const_int>) {
			return arg.type;
		}
		else if constexpr (std::is_same_v<T, mir_const_float>) {
			return arg.type;
		}
		else if constexpr (std::is_same_v<T, mir_string_imm>) {
			return arg.type;
		}
		else if constexpr (std::is_same_v<T, mir_nullptr>) {
			return mir_type{};
		}
		else if constexpr (std::is_same_v<T, mir_func_ref>) {
			return arg.function->return_type;
		}
		else if constexpr (std::is_same_v<T, mir_field_ref>) {
			// field refs do not have a type themselves
			// their type is determined by the context in which they are used
			return mir_type{};
		}
		else if constexpr (std::is_same_v<T, mir_block_ref>) {
			// block refs do not have a type
			return mir_type{};
		}
		else {
			return mir_type{};
		}
	}, op);
}