#include "name_path.h"

#include <string>

std::string name_path::flatten(const std::string& sep) const {
	std::string result;
	for (const auto& scope : scopes) {
		result += scope + sep;
	}
	result += name;
	return result;
}
