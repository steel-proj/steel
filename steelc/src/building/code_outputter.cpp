#include "code_outputter.h"

#include <filesystem>
#include <fstream>
#include <string>

#include <output/logging/log.h>

code_output_error code_outputter::output_code(const std::filesystem::path& path, const std::string& text_data) {
	return output_base(path, text_data, false);
}
code_output_error code_outputter::output_code(const std::filesystem::path& path, const std::vector<uint8_t>& binary_data) {
	std::string data_str(reinterpret_cast<const char*>(binary_data.data()), binary_data.size());
	return output_base(path, data_str, true);
}

code_output_error code_outputter::output_base(const std::filesystem::path& path, const std::string& data, bool binary) {
	// create directories (if nescessary)
	if (path.has_parent_path()) {
		std::error_code ec;
		std::filesystem::create_directories(path.parent_path(), ec);
		if (ec) {
			output::log::print("Failed to create directories for path: {}\n", path.parent_path());
			output::log::print("Error code: {} - {}\n", ec.value(), ec.message());
			return code_output_error::FAIL_CREATE_DIRECTORIES;
		}
	}

	std::ofstream file;
	if (binary) {
		file.open(path, std::ios::out | std::ios::binary);
	}
	else {
		file.open(path, std::ios::out);
	}

	if (!file || !file.is_open()) {
		return code_output_error::FAIL_CREATE_FILE;
	}

	// write data
	if (binary) {
		file.write(data.data(), static_cast<std::streamsize>(data.size()));
	}
	else {
		file << data;
	}

	file.close();

	return code_output_error::SUCCESS;
}
