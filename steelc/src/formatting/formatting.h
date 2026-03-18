#pragma once

#include <string>
#include <string_view>
#include <format>
#include <filesystem>

#include <formatting/std_formatters.h>
#include <formatting/styled_string.h>
#include <formatting/styled_paragraph.h>

// formatting.h
//
// this header contains formatting utility functions.
// mainly, it's a wrapper around std::format with a few
// steelc specific utilities and extensions.
//
// first of all, it supports formatting styled strings
// and styled paragraphs directly which avoids the need
// to deconstruct and reconstruct them.
//
// secondly, it ensures that all formatting extensions
// are available wherever formatting is used, without needing
// to include them all directly everytime.

namespace formatting {
	// basic std::string format
	template<typename... Args>
	inline std::string format(std::format_string<Args...> fmt, Args&&... args) {
		return std::vformat(fmt.get(), std::make_format_args(args...));
	}

	// basic std::string format (no compile time validation)
	template<typename... Args>
	inline std::string vformat(std::string_view fmt, Args&&... args) {
		return std::vformat(fmt, std::make_format_args(args...));
	}

	// styled_string format (retains styling information)
	template<typename... Args>
	inline styled_string format_styled(const styled_string& ss, Args&&... args) {
		return styled_string(ss.get_style(), std::vformat(ss.get_text(), std::make_format_args(args...)));
	}

	// styled_paragraph format (retains styling information)
	template<typename... Args>
	inline styled_paragraph format_paragraph(const styled_paragraph& sp, Args&&... args) {
		styled_paragraph result;
		for (const auto& segment : sp.get_segments()) {
			result << format(segment, args...);
		}
		return result;
	}

	std::string format_path(const std::filesystem::path& p, bool use_system_seperator = false);
}