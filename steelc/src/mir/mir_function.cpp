#include "mir_function.h"

#include <string>

#include <mir/mir_block.h>

mir_block* mir_function::get_entry_block() {
	if (blocks.empty()) {
		return nullptr;
	}
	return &blocks.front();
}

mir_block& mir_function::add_block(const std::string& name) {
	int next_block_index = static_cast<int>(blocks.size());
	blocks.push_back(mir_block(name.empty() ? "block" + std::to_string(next_block_index) : name, next_block_index));
	return blocks.back();
}

