#include "env.h"

#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>

#include <sys/host_defs.h>

bool sys::env::get_var(const std::string& name, std::string& out) {
#if defined(STEELC_PLATFORM_WINDOWS)
	char* buffer = nullptr;
	size_t size = 0;
	if (_dupenv_s(&buffer, &size, name.c_str()) == 0 && buffer != nullptr) {
		out = buffer;
		free(buffer);
		return true;
	}
	return false;
#else
	char* value = std::getenv(name.c_str());
	if (value != nullptr) {
		out = value;
		return true;
	}
	return false;
#endif
}
std::vector<std::string> sys::env::get_path_entries() {
	std::vector<std::string> entries;

	std::string path;
	if (!get_var("PATH", path)) {
		return entries; // PATH not found
	}

#if defined(STEELC_PLATFORM_WINDOWS)
	const char path_sep = ';';
#elif defined(STEELC_PLATFORM_LINUX) || defined(STEELC_PLATFORM_MAC)
	const char path_sep = ':';
#endif

	// split by platform separator
	std::string entry;
	std::istringstream path_iss(path);
	while (std::getline(path_iss, path, path_sep)) {
		entries.push_back(entry);
	}
	return entries;
}
