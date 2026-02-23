#pragma once

#include <string>
#include <vector>
#include <format>
#include <utility>

#include <error/compilation_error.h>
#include <formatting/text_style.h>
#include <formatting/styled_string.h>
#include <formatting/formatting.h>
#include <output/router.h>

namespace diagnostics {
	namespace detail {
		template<typename... Args>
		inline void output_fmt(text_style style, std::format_string<Args...> fmt, Args&&... args) {
			std::string formatted = formatting::format(fmt.get(), std::forward<Args>(args)...);
			output::router::instance().write(output::router::channel::DIAGNOSTICS, styled_string(style, formatted));
		}
	}

	template<typename... Args>
	inline void error(std::format_string<Args...> fmt, Args&&... args) {
		detail::output_fmt(text_styles::ERROR, fmt, std::forward<Args>(args)...);
	}
	template<typename... Args>
	inline void warn(std::format_string<Args...> fmt, Args&&... args) {
		detail::output_fmt(text_styles::WARNING, fmt, std::forward<Args>(args)...);
	}
	template<typename... Args>
	inline void note(std::format_string<Args...> fmt, Args&&... args) {
		detail::output_fmt(text_styles::NOTE, fmt, std::forward<Args>(args)...);
	}

	void print_compilation_errors(const std::vector<compilation_error>& errors);
}