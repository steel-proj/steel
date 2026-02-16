#pragma once

#include <string>
#include <deque>
#include <vector>
#include <cstdint>
#include <unordered_map>

#include <mir/mir_fwd.h>
#include <mir/mir_type.h>
#include <mir/mir_value.h>
#include <mir/mir_block.h>
#include <sys/calling_convention.h>

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

class mir_function {
public:
	mir_function() = default;

	mir_block* get_entry_block();

	mir_block& add_block(const std::string& name = "");

	std::string name;
	std::vector<std::string> scopes;
	mir_type return_type;
	std::vector<mir_type> generic_args;
	std::vector<mir_function_param> params;
	std::deque<mir_block> blocks;
	mir_function_flags flags = MIR_FUNC_NONE;
	calling_convention call_conv = calling_convention::CDECL;

private:
};