#pragma once

#include <string>
#include <format>

#include <formatting/text_style.h>
#include <formatting/styled_string.h>
#include <formatting/formatting.h>
#include <output/router.h>

namespace output {
	namespace log {
		namespace detail {
			template<typename... Args>
			inline void output_fmt(std::format_string<Args...> fmt, Args&&... args) {
				std::string formatted = formatting::format(fmt.get(), std::forward<Args>(args)...);
				router::instance().write(router::channel::LOG, styled_string(formatted));
			}
		}

		template<typename... Args>
		inline void print(std::format_string<Args...> fmt, Args&&... args) {
			detail::output_fmt(fmt, std::forward<Args>(args)...);
		}
	}
}