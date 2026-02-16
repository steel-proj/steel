#include "mir_value.h"

#include <unordered_map>

void mir_value::assign_name(const std::string& name) const {
	if (!valid()) {
		return;
	}
	value_names[id] = name;
}
std::string mir_value::get_name() const {
	if (!valid()) {
		return "<invalid>";
	}
	auto it = value_names.find(id);
	if (it != value_names.end()) {
		return it->second;
	}
	return std::to_string(id);
}

mir_value::id_type mir_value::next_value_id = 0;
std::unordered_map<mir_value::id_type, std::string> mir_value::value_names;
