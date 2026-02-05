#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

#include <mir/mir_fwd.h>
#include <mir/mir_type.h>
#include <mir/mir_value.h>
#include <mir/mir_block.h>

struct mir_function_param {
	mir_type type;
	std::string name;
	mir_value value;
};

typedef uint32_t mir_function_flags;
enum mir_function_flag : uint32_t {
	MIR_FUNC_NONE = 0,
	MIR_FUNC_NO_MANGLE = 1 << 0,
	MIR_FUNC_NO_BODY = 1 << 1,
};

class mir_function{
public:
	mir_function() = default;

	inline mir_block* get_entry_block() {
		if (blocks.empty()) {
			return nullptr;
		}
		return &blocks[0];
	}

	inline mir_value make_value(const mir_type& ty, const std::string& name = "") {
		return mir_value(next_value_id++, ty);
	}

	// for debugging and readability purposes
	inline void assign_value_name(const mir_value& value, const std::string& name) {
		value_names[value.get_id()] = name;
	}
	inline std::string get_value_name(const mir_value& value) const {
		if (value_names.contains(value.get_id())) {
			return value_names.at(value.get_id());
		}
		return std::to_string(value.get_id());
	}

	std::string name;
	std::vector<std::string> scopes;
	mir_type return_type;
	std::vector<mir_type> generic_args;
	std::vector<mir_function_param> params;
	std::vector<mir_block> blocks;
	mir_function_flags flags = MIR_FUNC_NONE;

private:
	uint32_t next_value_id = 0;
	std::unordered_map<mir_value::id_type, std::string> value_names;
};