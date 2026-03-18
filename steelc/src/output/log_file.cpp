#include "log_file.h"

#include <string>
#include <string_view>
#include <fstream>

bool log_file::open(const std::string& file_path) {
	_stream.open(file_path, std::ios::app);
	return _stream.is_open();
}
