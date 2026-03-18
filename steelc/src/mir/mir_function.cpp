#include "mir_function.h"

#include <string>
#include <vector>
#include <memory>

#include <mir/mir_block.h>

mir_block* mir_function::add_block(const std::string& block_name) {
	auto block = mir_block::create(block_name.empty() ? "block" + std::to_string(blocks.size()) : block_name);
	blocks.push_back(std::move(block));
	return blocks.back().get();
}
void mir_function::add_block(std::unique_ptr<mir_block> block) {
	blocks.push_back(std::move(block));
}
