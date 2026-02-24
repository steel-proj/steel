#pragma once

#include <string>
#include <filesystem>
#include <memory>
#include <vector>
#include <cstdint>

#include <building/build_config.h>

enum class code_output_error {
	SUCCESS = 0,
	FAIL_CREATE_DIRECTORIES,
	FAIL_CREATE_FILE,
};

class code_outputter {
public:
	inline static std::unique_ptr<code_outputter> create() {
		return std::unique_ptr<code_outputter>(new code_outputter());
	}

	// outputs textual data to the given path
	code_output_error output_code(const std::filesystem::path& path, const std::string& text_data);
	// outputs binary data to the given path
	code_output_error output_code(const std::filesystem::path& path, const std::vector<uint8_t>& binary_data);

private:
	code_outputter() = default;

	code_output_error output_base(const std::filesystem::path& full_path, const std::string& data, bool binary);
};