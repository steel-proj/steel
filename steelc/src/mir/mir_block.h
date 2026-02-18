#pragma once

#include <string>
#include <vector>

#include <mir/mir_instr.h>

// mir_block
//
// mir blocks are sequences of MIR instructions
// they represent basic blocks within a function
//
// notes:
// - index is -1 for blocks created without an owning function
// - mir_function is responsible for assigning the index to any said mir_block within it,
// hence its a friend class

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