#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <functional>

#include <formatting/text_style.h>

// style_string
//
// a simple wrapper around std::string that also stores
// the text_style ascociated with it.
// avoids storing the style within the raw string itself.

class styled_string {
public:
	styled_string() = default;
	styled_string(const std::string& str)
		: style(text_styles::DEFAULT), text(str), styled(false) { 
	}
	styled_string(std::string_view str)
		: style(text_styles::DEFAULT), text(str), styled(false) {
	}
	styled_string(const text_style style, const std::string& str)
		: style(style), text(str), styled(true) {
	}

	inline std::string& get_text() {
		return text;
	}
	inline const text_style& get_style() const {
		return style;
	}
	inline bool is_styled() const {
		return styled;
	}

	// returns the raw string without any styling information
	inline const std::string& raw() const {
		return text;
	}
	// returns a formatted version of the string using the provided style formatter function
	std::string format(std::function<std::string(const text_style&)> style_formatter) const {
		if (!styled) {
			return text;
		}
		return style_formatter(style) + text;
	}
	
private:
	text_style style;
	std::string text;
	bool styled = false;
};
