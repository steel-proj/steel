#pragma once

#include <string>
#include <string_view>
#include <cstdarg>
#include <vector>

namespace string_utils {
	std::string format(const std::string& format_str, va_list args);
	std::string vec_to_string(const std::vector<std::string>& vec, const std::string& separator = ", ");

	std::string pad_left(std::string_view str, size_t total_length, char padding_char = ' ');
	std::string pad_right(std::string_view str, size_t total_length, char padding_char = ' ');
}