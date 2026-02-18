#pragma once

#include <string>
#include <vector>

#include <mir/mir_instr.h>

// mir_block
//
// MIR blocks are sequences of MIR instructions
// they represent basic blocks within a function
//
// Notes:
// - To create a standalone block (without a parent function), use the static create() method.
// If you later want to add this to a function, use mir_function::add_block() - make sure to std::move()!
// (this allows for delayed block appending)
// 
// - To create a block in a function, use mir_function::add_block() instead,
// it is much simpler and returns a raw mir_block* instead of a unique_ptr.
//

class mir_block {
public:
	inline static std::unique_ptr<mir_block> create(const std::string& name = "") {
		return std::unique_ptr<mir_block>(new mir_block(name));
	}

	inline void push_instr(const mir_instr& instr) {
		instructions.push_back(instr);
	}
	inline const std::vector<mir_instr>& get_instrs() const {
		return instructions;
	}

	const mir_instr* get_terminator() const;

	std::string name;

private:
	mir_block(const std::string& name)
		: name(name) {
	}

	std::vector<mir_instr> instructions;
};