#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <cstdint>

class source_file {
public:
	source_file(const std::filesystem::path& path, const std::filesystem::path& path_relative);

	std::string name() const;

	// filesystem utilites
	uint64_t get_last_modified_time() const;
	uint64_t get_size() const;

	std::filesystem::path full_path;
	std::filesystem::path relative_path;
	std::vector<std::string> lines;
	std::string content;

private:
	std::string name_cache;
};