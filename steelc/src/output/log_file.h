#pragma once

#include <string>
#include <string_view>
#include <fstream>

class log_file {
public:
	log_file() = default;

	bool open(const std::string& file_path);
	std::ofstream& stream() { return _stream; }

private:
	std::string _path;
	std::ofstream _stream;
};