#pragma once

#include <format>
#include <utility>

#include <output/router.h>
#include <formatting/text_style.h>
#include <formatting/formatting.h>

namespace output {
	namespace detail {
		template<typename... Args>
		inline void output_fmt(text_style style, std::format_string<Args...> fmt, Args&&... args) {
			std::string formatted = formatting::format(fmt, std::forward<Args>(args)...);
			router::instance().write(router::channel::PRIMARY, styled_string(style, formatted));
		}
	}

	template<typename... Args>
	inline void print(std::format_string<Args...> fmt, Args&&... args) {
		detail::output_fmt(text_styles::DEFAULT, fmt, std::forward<Args>(args)...);
	}
	template<typename... Args>
	inline void print(text_style style, std::format_string<Args...> fmt, Args&&... args) {
		detail::output_fmt(style, fmt, std::forward<Args>(args)...);
	}
}