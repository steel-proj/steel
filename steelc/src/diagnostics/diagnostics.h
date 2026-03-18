#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <format>
#include <utility>

#include <diagnostics/compilation_diagnostic.h>
#include <formatting/text_style.h>
#include <formatting/styled_string.h>
#include <formatting/formatting.h>
#include <output/router.h>

namespace diagnostics {
	namespace detail {
		template<typename... Args>
		inline void output_fmt(text_style style, std::string_view fmt, Args&&... args) {
			std::string formatted = formatting::vformat(fmt, std::forward<Args>(args)...);
			output::router::instance().write(output::router::channel::DIAGNOSTICS, styled_string(style, formatted));
		}
	}

	template<typename... Args>
	inline void error(std::format_string<Args...> fmt, Args&&... args) {
		detail::output_fmt(text_styles::ERROR, fmt.get(), std::forward<Args>(args)...);
	}
	template<typename... Args>
	inline void warn(std::format_string<Args...> fmt, Args&&... args) {
		detail::output_fmt(text_styles::WARNING, fmt.get(), std::forward<Args>(args)...);
	}
	template<typename... Args>
	inline void note(std::format_string<Args...> fmt, Args&&... args) {
		detail::output_fmt(text_styles::NOTE, fmt.get(), std::forward<Args>(args)...);
	}

	void print_diagnostic(const compilation_diagnostic& diag);
}