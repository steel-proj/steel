#pragma once

#include <vector>

#include <mir/mir_fwd.h>

// loop_helper
//
// Helper class for lowering loops to MIR.
// 
// Mostly it maintains a stack of loop contexts,
// which is used to keep track of the current loop's
// condition and merge blocks, which are needed for
// break and continue statements.
//
// Notes:

struct loop_context {
	mir_block* cond_block;
	mir_block* merge_block;
};

class loop_helper {
public:
	loop_helper() = default;

	inline void push_loop(mir_block* cond_block, mir_block* merge_block) {
		context_stack.push_back({cond_block, merge_block});
	}
	inline void pop_loop() {
		context_stack.pop_back();
	}

	inline bool in_loop() const {
		return !context_stack.empty();
	}
	inline loop_context* current_loop() {
		if (context_stack.empty()) {
			return nullptr;
		}
		return &context_stack.back();
	}

private:
	std::vector<loop_context> context_stack;
};