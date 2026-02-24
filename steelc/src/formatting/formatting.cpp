#include "formatting.h"

#include <string>
#include <filesystem>

std::string formatting::format_path(const std::filesystem::path& p, bool use_system_seperator) {
	auto ln = p.lexically_normal();

	if (!use_system_seperator) {
		return ln.generic_string();
	}
	else {
		return ln.make_preferred().string();
	}
}