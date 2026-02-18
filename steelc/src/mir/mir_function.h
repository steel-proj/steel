#pragma once

#include <string>
#include <deque>
#include <memory>
#include <cstdint>

#include <mir/mir_fwd.h>
#include <mir/mir_type.h>
#include <mir/mir_value.h>
#include <mir/mir_block.h>
#include <sys/calling_convention.h>
#include <utils/name_path.h>

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

// mir_function
//
// an mir_function represents a Steel function and is made
// up of a sequence of mir blocks
//
// notes:
// - due to how block indexing works, blocks can only be added to the end,
// and cannot be moved or removed

class mir_function {
public:
	inline static std::unique_ptr<mir_function> create(
		const name_path& name,
		mir_type return_type,
		const std::vector<mir_type>& generic_args,
		const std::vector<mir_function_param>& params,
		mir_function_flags flags = MIR_FUNC_NONE,
		calling_convention call_conv = calling_convention::CDECL)
	{
		auto fn = std::unique_ptr<mir_function>(new mir_function());
		fn->name = name;
		fn->return_type = return_type;
		fn->generic_args = generic_args;
		fn->params = params;
		fn->flags = flags;
		fn->call_conv = call_conv;
		return fn;
	}

	mir_block* get_entry_block();

	inline bool empty() const {
		return blocks.empty();
	}

	inline const std::vector<std::unique_ptr<mir_block>>& get_blocks() const {
		return blocks;
	}
	inline mir_block* at(int index) {
		return blocks[index].get();
	}
	mir_block* add_block(const std::string& name = "");
	void add_block(std::unique_ptr<mir_block> block);

	name_path name;
	mir_type return_type;
	std::vector<mir_type> generic_args;
	std::vector<mir_function_param> params;
	mir_function_flags flags = MIR_FUNC_NONE;
	calling_convention call_conv = calling_convention::CDECL;

private:
	mir_function() = default;

	std::vector<std::unique_ptr<mir_block>> blocks;
};