#include "mir_function.h"

mir_block* mir_function::get_entry_block() {
	if (blocks.empty()) {
		return nullptr;
	}
	return &blocks.front();
}

mir_value mir_function::make_value(const mir_type& ty, const std::string& name) {
	return mir_value(next_value_id++, ty);
}

mir_block& mir_function::add_block(const std::string& name) {
	int next_block_index = static_cast<int>(blocks.size());
	blocks.push_back(mir_block(name.empty() ? "block" + std::to_string(next_block_index) : name, next_block_index));
	return blocks.back();
}

void mir_function::assign_value_name(const mir_value& value, const std::string& name) {
	value_names[value.get_id()] = name;
}
std::string mir_function::get_value_name(const mir_value& value) const {
	auto it = value_names.find(value.get_id());
	if (it != value_names.end()) {
		return it->second;
	}
	return std::to_string(value.get_id());
}
