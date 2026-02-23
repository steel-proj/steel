#pragma once

#include <string>
#include <string_view>
#include <fstream>

class log_file {
public:
	log_file() = default;
	~log_file();

	bool open(const std::string& file_path);
	std::ofstream& stream() { return file_stream; }

private:
	std::string file_path;
	std::ofstream file_stream;
};