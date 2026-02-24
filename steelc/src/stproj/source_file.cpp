#include "source_file.h"

#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <string>
#include <cstdint>

#include <formatting/formatting.h>

source_file::source_file(const std::filesystem::path& path, const std::filesystem::path& path_relative) {
	this->full_path = formatting::format_path(path);
	this->relative_path = formatting::format_path(path_relative);

	std::ifstream file(full_path);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			lines.push_back(line);
			content += line + "\n";
		}
		file.close();

		name_cache = full_path.filename().string();
	}
	else {
		throw std::runtime_error("Could not open source file: " + relative_path.string());
	}
}

std::string source_file::name() const {
	return name_cache;
}

uint64_t source_file::get_last_modified_time() const {
	return static_cast<uint64_t>(std::filesystem::last_write_time(full_path).time_since_epoch().count());
}
uint64_t source_file::get_size() const {
	return static_cast<uint64_t>(std::filesystem::file_size(full_path));
}
