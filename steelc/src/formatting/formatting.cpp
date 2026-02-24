#include "formatting.h"

#include <string>
#include <filesystem>

std::string formatting::format_path(const std::filesystem::path& p) {
	std::u8string pretty = p.lexically_normal().make_preferred().u8string();
	return std::string(pretty.begin(), pretty.end());
}