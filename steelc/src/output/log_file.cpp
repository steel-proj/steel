#include "log_file.h"

#include <string>
#include <string_view>
#include <fstream>

log_file::~log_file() {
	if (file_stream.is_open()) {
		file_stream.close();
	}
}

bool log_file::open(const std::string& file_path) {
	file_stream = std::ofstream(file_path, std::ios::app);
	return file_stream.is_open();
}
